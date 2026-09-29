<?php

namespace Serenata\UserInterface;

use React;
use Closure;
use DomainException;
use RuntimeException;
use UnexpectedValueException;

use React\EventLoop\LoopInterface;
use React\EventLoop\TimerInterface;

use Serenata\Sockets\JsonRpcQueue;
use Serenata\Sockets\SocketServer;
use Serenata\Sockets\JsonRpcRequest;
use Serenata\Sockets\JsonRpcResponse;
use Serenata\Sockets\JsonRpcQueueItem;
use Serenata\Sockets\JsonRpcRequestSender;
use Serenata\Sockets\JsonRpcMessageInterface;
use Serenata\Sockets\JsonRpcQueueItemProcessor;
use Serenata\Sockets\JsonRpcMessageHandlerInterface;
use Serenata\Sockets\JsonRpcMessageSenderInterface;
use Serenata\Sockets\JsonRpcConnectionHandlerFactory;

use Symfony\Component\Console\Application;

use Symfony\Component\Console\Input\InputOption;
use Symfony\Component\Console\Input\InputInterface;

use Symfony\Component\Console\Output\OutputInterface;

/**
 * Application extension that can handle JSON-RPC requests.
 */
final class JsonRpcApplication extends AbstractApplication implements JsonRpcMessageHandlerInterface
{
    /**
     * @var float
     */
    private const REQUEST_HANDLE_FREQUENCY_SECONDS = 0;

    /**
     * @var int
     */
    private const CYCLE_COLLECTION_FREQUENCY_SECONDS = 5;

    /**
     * @var TimerInterface|null
     */
    private $periodicQueueProcessingTimer;

    /**
     * @inheritDoc
     */
    public function run()
    {
        $application = (new Application('PHP Integrator Core'))
            ->register('start')
                ->addOption('uri', 'u', InputOption::VALUE_OPTIONAL, 'The URI to run on', null)
                ->setCode(Closure::fromCallable([$this, 'runEventLoop']))
            ->getApplication();

        assert($application !== null);

        $application->setAutoExit(false);
        $application->setDefaultCommand('start', true);

        return $application->run();
    }

    /**
     * @inheritDoc
     */
    public function handle(JsonRpcMessageInterface $message, JsonRpcMessageSenderInterface $jsonRpcMessageSender): void
    {
        if ($message instanceof JsonRpcRequest) {
            $class = \get_class($message);
            \gDebug("JsonRpcRequest({$class}): ".json_encode($message->jsonSerialize()));
            /** @var JsonRpcQueue $queue */
            $queue = $this->getContainer()->get(JsonRpcQueue::class);
            $queue->push(new JsonRpcQueueItem($message, $jsonRpcMessageSender));

            $this->ensurePeriodicQueueProcessingTimerIsInstalled();
        } elseif ($message instanceof JsonRpcResponse) {
            \gDebug("JsonRpcResponse: ".json_encode($message->jsonSerialize()));
            /** @var JsonRpcRequestSender $requestSender */
            $requestSender = $this->getContainer()->get(JsonRpcRequestSender::class);
            $requestSender->handleResponse($message);
        } else {
            throw new DomainException(
                'Received JSON-RPC message that is neither a request nor a response, but a "' . get_class($message) .
                '" instead, which is not supportedl'
            );
        }
    }

    /**
     * phpcs:disable -- runEventLoop is called with array syntax above and not an unused method.
     *
     * @param InputInterface  $input
     * @param OutputInterface $output
     *
     * @return int
     */
    private function runEventLoop(InputInterface $input, OutputInterface $output): int
    {
        // phpcs:enable
        $uri = $input->getOption('uri');

        if ($uri === null) {
            throw new UnexpectedValueException('A URI for handling requests must be specified');
        } elseif (!is_string($uri)) {
            throw new UnexpectedValueException('The URI for handling requests must be a string');
        }

        /** @var LoopInterface $loop */
        $loop = $this->getContainer()->get('eventLoop');

        try {
            $this->setupRequestHandlingSocketServer($loop, $uri);
        } catch (RuntimeException $e) {
            $output->writeln("<error>Could not bind to socket on URI {$uri}</>");

            return 2;
        }

        $output->writeln("<info>Starting server bound to socket on URI {$uri}...</>");

        $this->instantiateRequiredServices($this->getContainer());

        $loop->run();

        return 0;
    }

    /**
     * @return void
     */
    private function ensurePeriodicQueueProcessingTimerIsInstalled(): void
    {
        if ($this->periodicQueueProcessingTimer !== null) {
            return;
        }

        $this->installPeriodicQueueProcessingTimer();
    }

    /**
     * @return void
     */
    private function installPeriodicQueueProcessingTimer(): void
    {
        /** @var LoopInterface $loop */
        $loop = $this->getContainer()->get('eventLoop');

        $this->periodicQueueProcessingTimer = $loop->addPeriodicTimer(
            self::REQUEST_HANDLE_FREQUENCY_SECONDS,
            function (): void {
                $this->processNextQueueItem();

                /** @var JsonRpcQueue $queue */
                $queue = $this->getContainer()->get(JsonRpcQueue::class);

                if ($queue->isEmpty()) {
                    $this->uninstallPeriodicQueueProcessingTimer();
                }
            }
        );
    }

    /**
     * @return void
     */
    private function uninstallPeriodicQueueProcessingTimer(): void
    {
        if ($this->periodicQueueProcessingTimer !== null) {
            /** @var LoopInterface $loop */
            $loop = $this->getContainer()->get('eventLoop');
            $loop->cancelTimer($this->periodicQueueProcessingTimer);

            $this->periodicQueueProcessingTimer = null;
        }
    }

    /**
     * @return void
     */
    private function processNextQueueItem(): void
    {
        /** @var JsonRpcQueue $queue */
        $queue = $this->getContainer()->get(JsonRpcQueue::class);

        /** @var JsonRpcQueueItemProcessor $processor */
        $processor = $this->getContainer()->get(JsonRpcQueueItemProcessor::class);
        $processor->process($queue->pop());
    }

    /**
     * @param React\EventLoop\LoopInterface $loop
     * @param string                        $uri
     *
     * @throws RuntimeException
     *
     * @return void
     */
    private function setupRequestHandlingSocketServer(React\EventLoop\LoopInterface $loop, string $uri): void
    {
        $connectionHandlerFactory = new JsonRpcConnectionHandlerFactory($this);

        $requestHandlingSocketServer = new SocketServer($uri, $loop, $connectionHandlerFactory);

        /** @var LoopInterface $loop */
        $loop = $this->getContainer()->get('eventLoop');
        $loop->addPeriodicTimer(
            self::CYCLE_COLLECTION_FREQUENCY_SECONDS,
            function (): void {
                // Still try to collect cyclic references every so often. See also Bootstrap.php for the reasoning.
                // Do *not* do this after every request handle as it puts a major strain on performance, especially
                // during project indexing. Also don't cancel this timer when the last request is handled, as during
                // normal usage, the frequency may be too high to ever trigger before it is cancelled.
                gc_collect_cycles();
            }
        );
    }
}
