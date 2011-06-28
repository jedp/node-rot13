/*
 * sync/async rot13 encoder
 */
 
#include <v8.h>
#include <node.h>
#include <iostream>
 
using namespace node;
using namespace v8;
using namespace std;

// macros from pquerna's https://github.com/pquerna/node-extension-examples

#define REQ_STR_ARG(I, VAR)                                             \
    if (args.Length() <= (I) || !args[I]->IsString())                   \
        return ThrowException(Exception::TypeError(                     \
                String::New("Argument " #I " must be a string")));      \
    Local<String> VAR = Local<String>::Cast(args[I])        

#define REQ_FUN_ARG(I, VAR)                                             \
    if (args.Length() <= (I) || !args[I]->IsFunction())                 \
        return ThrowException(Exception::TypeError(                     \
                String::New("Argument " #I " must be a function")));    \
    Local<Function> VAR = Local<Function>::Cast(args[I])        

// Convert a v8 value to a std string
string ObjectToString(Local<Value> value) {
    String::Utf8Value utf8_value(value);
    return std::string(*utf8_value);
}

char rotate_ch(const char ch)
{
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
        NODE_SET_PROTOTYPE_METHOD(s_ct, "rotateAsync", RotateAsync);

        // expose class as Rot13
        target->Set(String::NewSymbol("Rot13"), s_ct->GetFunction());
    }

    Rot13() 
    {
    }

    ~Rot13()
    {
    }

    // A baton to hand off references through the eio cycle
    struct baton_t {
        Rot13 *rot13;
        std::string source;
        std::string rotated;
        Persistent<Function> cb;
    };

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

    static Handle<Value> RotateAsync(const Arguments &args) 
    {
        // Caller: rotateAsync(s, cb) 
        // This function: cb(rotated(s))
        REQ_STR_ARG(0, s);
        REQ_FUN_ARG(1, cb);

        Rot13 *rot13 = ObjectWrap::Unwrap<Rot13>(args.This());

        baton_t *baton = new baton_t();
        baton->rot13 = rot13;
        baton->source = ObjectToString(s);
        baton->cb = Persistent<Function>::New(cb);

        // Add refcount so we won't get gc'd while running in another thread
        rot13->Ref();

        // eio_custom will cause EIO_Rotate to be run in the thread pool,
        // and EIO_AfterRotate to be run back in the main thread.  Each
        // function will have the baton passed to it.
        eio_custom(EIO_Rotate, EIO_PRI_DEFAULT, EIO_AfterRotate, baton);

        // Increase the ref count to the event loop, so node doesn't exit
        // because it thinks there are no more events to handle.
        ev_ref(EV_DEFAULT_UC);

        return Undefined();
    }

    static int EIO_Rotate(eio_req *req)
    {
        // runs in the thread pool
        // nothing here should use js or v8
       
        baton_t *baton = static_cast<baton_t *>(req->data);

        baton->rotated = rotate_str(baton->source);

        // when this function returns, libeio will notify the main
        // thread that EIO_AfterRotate sould run.
        return 0;
    }

    static int EIO_AfterRotate(eio_req *req)
    {
        // runs back in the main thread
        baton_t *baton = static_cast<baton_t *>(req->data);

        // decrement references we incremented in Rotate()
        ev_unref(EV_DEFAULT_UC);
        baton->rot13->Unref();

        // prepare args for callback
        Local<Value> argv[1];
        argv[0] = String::New(baton->rotated.c_str());

        TryCatch try_catch;
        baton->cb->Call(Context::GetCurrent()->Global(), 1, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }

        // Having run the callback and caught any exceptions, we
        // destroy the persistent reference and the baton.
        baton->cb.Dispose();
        delete baton;
        return 0;
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
