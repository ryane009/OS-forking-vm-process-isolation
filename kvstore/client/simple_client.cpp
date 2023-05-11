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

/*
In this function, given the person in question, to respect their wishes 
on the right to be forgotten, we redact their name whenever either
their first or last name shows up in a post. We redact the name by 
parsing through all the different posts made by every user, and replacing 
matches with the user request's name with a "*". 

NB: For words like matter, if the user has the name "Matt," it will redact to *er
*/
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

  //using strtok to parse through the string that is returned
  //from the get, and adding it to our working vector of all
  //user ids
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
  //In the case of 5B, we should have two names in the vector
  //that we'll later search and redact for: Matt and Bleat
  //in addition, as checking for matches are case sensitive,
  //we preemptively change each of the std::strings to lowercase
  char* temp1 = strdup(name.value().c_str());
  char* name_token;
  std::vector<std::string> name_parts;

  //parsing to retrieve different names to search for in our post archive
  name_token = std::strtok(temp1, " "); 
  while(name_token != NULL){
    std::string str(name_token);
    std::string result;

    //using a transformer function to convert each String to lower case
    std::transform(str.begin(), str.end(), std::back_inserter(result),
                   [](unsigned char c){ return std::tolower(c); });
    name_parts.push_back(result);
    name_token = std::strtok(NULL, " ");
  }
  delete[] temp1;

  //retireiving all the post_ids from every post that our users made

  //iterating through all of our users using the vector that we populated
  for(int i = 0; i < user_ids.size(); i++){
    std::vector<std::string> post_ids;
    std::string key = user_ids[i];
    //adding posts so that we can have the key to access a given user's post_id
    key += "_posts";

    //strtok again
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

    //now retrieiving every post, and editing it as we need to
    for(int i = 0; i < post_ids.size(); i++){
      std::optional<std::string> content = Get(post_ids[i]);
      if (content.has_value()) {
        //we copy contents of post into text, so we can convert everything into
        //lowercase in text without changing the original content in a way 
        //where everything becomes lower case (we're only redacting, not changing
        //the text content). We need to convert the text into
        //lower-case however so that we can compare the text with name of the user we retrieved above
        //without uppercase/lowercase getting in the way, so our checking for the user 
        //request's name non-casesensitive
        std::string text;
        std::transform(content.value().begin(), content.value().end(), std::back_inserter(text),
                    [](unsigned char c){ return std::tolower(c); });
        for(int n = 0; n < name_parts.size(); n++){
          size_t index = text.find(name_parts[n]);

          //while iterating through a 
          while(index != std::string::npos){

            //redacting actual content on the page
            content.value().erase(index, name_parts[n].size());
            content.value().insert(index, "*");

            //making those changes in text too, so we don't double update and the
            //indexes of the modified text and actual content are still synced
            text.erase(index, name_parts[n].size());
            text.insert(index, "*");
            index = text.find(name_parts[n], index + 1);
          }
        }
        //updating post with new post (could be identifical, or feature redactions)
        Put(post_ids[i], content.value());
      }
    }

  }

  //Put(const std::string& key, const std::string& value)
  // cerr_color(RED, "GDPR deletion is unimplemented!");
  return true;
}
