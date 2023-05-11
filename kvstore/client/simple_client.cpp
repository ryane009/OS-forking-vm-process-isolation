#include "simple_client.hpp"
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <cctype>


std::optional<std::string> SimpleClient::Get(const std::string& key) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return std::nullopt;
  }

  GetRequest req{key};
  if (!conn->send_request(req)) return std::nullopt;

  std::optional<Response> res = conn->recv_response();
  if (!res) return std::nullopt;
  if (auto* get_res = std::get_if<GetResponse>(&*res)) {
    return get_res->value;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Get value from server: ", error_res->msg);
  }

  return std::nullopt;
}

bool SimpleClient::Put(const std::string& key, const std::string& value) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return false;
  }

  PutRequest req{key, value};
  if (!conn->send_request(req)) return false;

  std::optional<Response> res = conn->recv_response();
  if (!res) return false;
  if (auto* put_res = std::get_if<PutResponse>(&*res)) {
    return true;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Put value to server: ", error_res->msg);
  }

  return false;
}

bool SimpleClient::Append(const std::string& key, const std::string& value) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return false;
  }

  AppendRequest req{key, value};
  if (!conn->send_request(req)) return false;

  std::optional<Response> res = conn->recv_response();
  if (!res) return false;
  if (auto* append_res = std::get_if<AppendResponse>(&*res)) {
    return true;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Append value to server: ", error_res->msg);
  }

  return false;
}

std::optional<std::string> SimpleClient::Delete(const std::string& key) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return std::nullopt;
  }

  DeleteRequest req{key};
  if (!conn->send_request(req)) return std::nullopt;

  std::optional<Response> res = conn->recv_response();
  if (!res) return std::nullopt;
  if (auto* delete_res = std::get_if<DeleteResponse>(&*res)) {
    return delete_res->value;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Delete value on server: ", error_res->msg);
  }

  return std::nullopt;
}

std::optional<std::vector<std::string>> SimpleClient::MultiGet(
    const std::vector<std::string>& keys) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return std::nullopt;
  }

  MultiGetRequest req{keys};
  if (!conn->send_request(req)) return std::nullopt;

  std::optional<Response> res = conn->recv_response();
  if (!res) return std::nullopt;
  if (auto* multiget_res = std::get_if<MultiGetResponse>(&*res)) {
    return multiget_res->values;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to MultiGet values on server: ", error_res->msg);
  }

  return std::nullopt;
}

bool SimpleClient::MultiPut(const std::vector<std::string>& keys,
                            const std::vector<std::string>& values) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return false;
  }

  MultiPutRequest req{keys, values};
  if (!conn->send_request(req)) return false;

  std::optional<Response> res = conn->recv_response();
  if (!res) return false;
  if (auto* multiput_res = std::get_if<MultiPutResponse>(&*res)) {
    return true;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to MultiPut values on server: ", error_res->msg);
  }

  return false;
}

bool SimpleClient::GDPRDelete(const std::string& user) {
  // TODO: Write your GDPR deletion code here!
  // You can invoke operations directly on the client object, like so:
  //
  // std::string key("user_1_posts");
  // std::optional<std::string> posts = Get(key);
  // ...
  //
  // Assume the `user` arugment is a user ID such as "user_1".

  //getting all users so we can go through all their posts
  std::optional<std::string> users = Get("all_users");
  std::vector<std::string> user_ids;

  char* temp = strdup(users.value().c_str());
  temp[strlen(temp)-1] = '\0';
  char* token1;
  token1 = std::strtok(temp, ","); 
  while(token1 != NULL){
    user_ids.push_back(token1);
    token1 = strtok(NULL, ",");
  }
  delete[] temp;


  //retrieving user's name
  std::optional<std::string> name = Get(user);

  //retrieving parts of user name that we want to filter for
  char* temp1 = strdup(name.value().c_str());
  char* name_token;
  std::vector<std::string> name_parts;
  name_token = std::strtok(temp1, " "); 
  while(name_token != NULL){
    std::string str(name_token);
    std::string result;
    std::transform(str.begin(), str.end(), std::back_inserter(result),
                   [](unsigned char c){ return std::tolower(c); });
    name_parts.push_back(result);
    name_token = std::strtok(NULL, " ");
  }
  delete[] temp1;

  for(int i = 0; i < user_ids.size(); i++){
    std::vector<std::string> post_ids;
    std::string key = user_ids[i];
    key += "_posts";
    std::optional<std::string> posts = Get(key);
    char* temp2 = strdup(posts.value().c_str());
    temp2[strlen(temp2)-1] = '\0';
    char* token2;
    token2 = std::strtok(temp2, " ,"); 
    while(token2 != NULL){
      post_ids.push_back(token2);
      token2 = std::strtok(NULL, " ,");
    }
    delete[] temp2;

    for(int i = 0; i < name_parts.size(); i++){
      std::cout << "testing post key " + name_parts[i] << std::endl;
    }

    // for(int i = 0; i < post_ids.size(); i++){
    //   std::optional<std::string> content = Get(post_ids[i]);
    //   std::string text;
    //   std::transform(content.value().begin(), content.value().end(), std::back_inserter(text),
    //                [](unsigned char c){ return std::tolower(c); });
    //   for(int n = 0; n < name_parts.size(); n++){
    //     size_t index = text.find(name_parts[n]);
    //     while(index != std::string::npos){
    //       content.value().erase(index, name_parts[n].size());
    //       content.value().insert(index, "*");
    //       index = text.find(name_parts[n]);
    //     }
    //   }
    //   Put(post_ids[i], content.value());
    // }
      for(int i = 0; i < post_ids.size(); i++){
        std::optional<std::string> content = Get(post_ids[i]);
        if (content.has_value()) {
          std::string text;
          std::transform(content.value().begin(), content.value().end(), std::back_inserter(text),
                      [](unsigned char c){ return std::tolower(c); });
          for(int n = 0; n < name_parts.size(); n++){
            size_t index = text.find(name_parts[n]);
            while(index != std::string::npos){
              text.erase(index, name_parts[n].size());
              text.insert(index, "*");
              index = text.find(name_parts[n], index + 1);
            }
          }
          Put(post_ids[i], text);
        }
      }

  }

  //Put(const std::string& key, const std::string& value)
  // cerr_color(RED, "GDPR deletion is unimplemented!");
  return true;
}
