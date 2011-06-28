
#include <v8.h>
#include <node.h>
#include <iostream>
 
using namespace node;
using namespace v8;
using namespace std;

// macro from pquerna's https://github.com/pquerna/node-extension-examples
#define REQ_STR_ARG(I, VAR)                                             \
    if (args.Length() <= (I) || !args[I]->IsString())                   \
        return ThrowException(Exception::TypeError(                     \
                String::New("Argument " #I " must be a string")));      \
    Local<String> VAR = Local<String>::Cast(args[I])        

// Convert a v8 value to a std string
string ObjectToString(Local<Value> value) {
    String::Utf8Value utf8_value(value);
    return std::string(*utf8_value);
}

char rotate_ch(const char ch)
{
    // a-z -> n-m
    if (97 <= ch && ch <= 122) {
        return ( (ch-97 + 13) % 26 + 97 );
    }

    // A-Z -> N-M
    else if (65 <= ch && ch <= 90) {
        return ( (ch-65 + 13) % 26 + 65 );
    }

    // default case, no change
    return (ch);
}

static Handle<Value> rotate(const Arguments &args)
{
    REQ_STR_ARG(0, s);

    std::string source = ObjectToString(s);
    std::string rotated;
    rotated.reserve(source.length());
    
    for (std::string::iterator it = source.begin(); it != source.end(); ++it) {
        rotated.push_back(rotate_ch(*it));
    }

    // Return the rotated string as a js string
    return (String::New(rotated.c_str()));
}
 
extern "C" {
    static void init(Handle<Object> target)
    {
        NODE_SET_METHOD(target, "rotate", rotate);
    }
    NODE_MODULE(rot13, init);
}
