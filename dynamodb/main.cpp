#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/PutItemResult.h>

#include <exception>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

using std::string;

class DynamoDB {
  Aws::Client::ClientConfiguration clientConfig;
  Aws::DynamoDB::DynamoDBClient dynamoClient;

 public:
  DynamoDB() : dynamoClient(clientConfig) {}
  void insert(string table, std::map<string, string> attributes) {
    using namespace Aws::DynamoDB::Model;
    PutItemRequest request;
    request.SetTableName(table);

    for (auto& [key, value] : attributes)
      request.AddItem(key, AttributeValue(value));

    PutItemOutcome result = dynamoClient.PutItem(request);
    if (!result.IsSuccess())
      throw std::runtime_error(result.GetError().GetMessage());
  }
};

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cout
        << "Usage:\n"
        << "    " << argv[0] << " <table> <name> <value>\n\n"
        << "Where:\n"
        << "    table    - the table to put the item in.\n"
        << "    name     - a name to add to the table. If the name already\n"
        << "               exists, its entry will be updated.\n\n"
        << "Example:\n"
        << "    " << argv[0] << " test Fulano 'Language=ca&Born=1876'\n";
    return 1;
  }

  Aws::SDKOptions options;

  Aws::InitAPI(options);
  {
    try {
      DynamoDB db;
      db.insert(string(argv[1]),
                {{"Name", string(argv[2])}, {"Value", string(argv[3])}});
      std::cout << "Done!" << std::endl;
    } catch (std::exception& ex) {
      std::cerr << ex.what() << std::endl;
    }
  }
  Aws::ShutdownAPI(options);
  return 0;
}
