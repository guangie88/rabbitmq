#include "gtest/gtest.h"

#include "SimpleAmqpClient/SimpleAmqpClient.h"

#include <iostream>
#include <string>

// SimpleAmqpClient
using AmqpClient::BasicMessage;
using AmqpClient::Channel;
using AmqpClient::Envelope;

// std
using std::cout;
using std::string;

static constexpr auto HOSTNAME = "hikari";
static constexpr auto QUEUE = "c++-queue";

TEST(Main, PublishConsumeRaii)
{
    // common message
    const auto message = BasicMessage::Create("This is C++");

    // not passive (false) -> queue is created even if it does not exists
    // durable (true) -> survives broker restart
    // not exclusive (false) -> queue does not get deleted after the connection is closed
    // not auto delete (false) -> queue does not get deleted after at least one exchange has been bound to it

    // publish
    {
        const auto channel = Channel::Create(HOSTNAME);
        const auto queue = channel->DeclareQueue(QUEUE, false, true, false, false);

        // publish
        channel->BasicPublish("", queue, message);
    }

    // consume
    {
        const auto channel = Channel::Create(HOSTNAME);
        const auto queue = channel->DeclareQueue(QUEUE, false, true, false, false);

        Envelope::ptr_t delivered;
        const auto consumer = channel->BasicConsume(queue);

        EXPECT_TRUE(channel->BasicConsumeMessage(consumer, delivered));
        EXPECT_EQ(consumer, delivered->ConsumerTag());
        EXPECT_EQ("", delivered->Exchange());
        EXPECT_EQ(queue, delivered->RoutingKey());
        EXPECT_EQ(message->Body(), delivered->Message()->Body());
    }
}

int main(int argc, char * argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}