
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
    // Rotate a character

    // a-z -> n-m
    // Lowercase letters more common, so check them first.
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

std::string rotate_str(const std::string &source)
{
    // Rotate the characters in a string 
    
    std::string rotated;
    rotated.reserve(source.length());

    // Rotate the characters in the string
    for (std::string::const_iterator it = source.begin(); it != source.end(); ++it) {
        rotated.push_back(rotate_ch(*it));
    }

    return (rotated);
}

class Rot13: ObjectWrap 
{
public:

    static Persistent<FunctionTemplate> s_ct;

    static void Init(Handle<Object> target)
    {
        s_ct = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
        s_ct->InstanceTemplate()->SetInternalFieldCount(1);
        s_ct->SetClassName(String::NewSymbol("Rot13"));

        // bind methods
        NODE_SET_PROTOTYPE_METHOD(s_ct, "rotate", Rotate);

        // expose class as Rot13
        target->Set(String::NewSymbol("Rot13"), s_ct->GetFunction());
    }

    Rot13() 
    {
    }

    ~Rot13()
    {
    }

    static Handle<Value> New(const Arguments &args)
    {
        Rot13 *rot13 = new Rot13();
        rot13->Wrap(args.This());
        return args.This();
    }

    static Handle<Value> Rotate(const Arguments &args)
    {
        // Rotate(s) -> rotated
        //
        // Given a string argument, rotate all the characters
        // in it and return the result.
        
        HandleScope scope;

        REQ_STR_ARG(0, s);

        // Convert to std::string for rotating, then back into v8 String 
        Local<String> result = String::New(rotate_str(ObjectToString(s)).c_str());
        return scope.Close(result);
    }
};
 

Persistent<FunctionTemplate> Rot13::s_ct;
extern "C" {
    static void init(Handle<Object> target)
    {
        Rot13::Init(target);
    }
    NODE_MODULE(rot13, init);
}
