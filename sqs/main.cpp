#include <aws/core/Aws.h>
#include <aws/sqs/SQSClient.h>
#include <aws/sqs/model/DeleteMessageRequest.h>
#include <aws/sqs/model/ReceiveMessageRequest.h>
#include <aws/sqs/model/ReceiveMessageResult.h>

#include <iostream>
#include <list>
#include <memory>
#include <stdexcept>

using Aws::SQS::Model::Message;

class Sqs {
  const std::string& queue_url;
  Aws::SQS::SQSClient sqs;

 public:
  Sqs(Aws::Client::ClientConfiguration& config, const Aws::String& queue_url)
      : queue_url(queue_url), sqs(config) {}

  Message ReceiveMessage() {
    Aws::SQS::Model::ReceiveMessageRequest rm_req;
    rm_req.SetQueueUrl(queue_url);
    rm_req.SetMaxNumberOfMessages(1);

    auto rm_out = sqs.ReceiveMessage(rm_req);
    if (!rm_out.IsSuccess())
      throw std::runtime_error("Error receiving message from queue ");

    const auto& messages = rm_out.GetResult().GetMessages();
    if (messages.size() == 0)
      throw std::runtime_error("No messages received from queue ");

    return messages[0];
  }

  void DeleteMessage(const Message& message) {
    Aws::SQS::Model::DeleteMessageRequest dm_req;
    dm_req.SetQueueUrl(queue_url);
    dm_req.SetReceiptHandle(message.GetReceiptHandle());

    auto dm_out = sqs.DeleteMessage(dm_req);
    if (!dm_out.IsSuccess())
      throw std::runtime_error(dm_out.GetError().GetMessage());
  }
};

void ConsumeMessage(Aws::Client::ClientConfiguration& config,
                    const Aws::String& queue_url) {
  try {
    const long MAX_LONG_POOLING_QUEUES_TIME = 30000L;
    config.requestTimeoutMs = MAX_LONG_POOLING_QUEUES_TIME;
    Sqs sqs(config, queue_url);

    const auto& message = sqs.ReceiveMessage();
    std::cout << "Received message:\n"
              << "\n  MessageId: " << message.GetMessageId()
              << "\n  ReceiptHandle: " << message.GetReceiptHandle()
              << "\n  Body: " << message.GetBody() << std::endl;

    sqs.DeleteMessage(message);
    std::cout << "Successfully deleted message " << message.GetMessageId()
              << " from queue " << queue_url << std::endl;

  } catch (std::runtime_error& error) {
    std::cerr << error.what() << " " << queue_url << std::endl;
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Usage: receive_message <queue_url>" << std::endl;
    return 1;
  }

  Aws::SDKOptions options;
  Aws::InitAPI(options);
  {
    Aws::Client::ClientConfiguration config;
    Aws::String queue_url = argv[1];
    ConsumeMessage(config, queue_url);
  }
  Aws::ShutdownAPI(options);
  return 0;
}
