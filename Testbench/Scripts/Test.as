void main()
{
    // Print a message
    Ryu::Log(Ryu::LogLevel::Trace, "Hello Trace");
    Ryu::Log(Ryu::LogLevel::Debug, "Hello Debug");
    Ryu::Log(Ryu::LogLevel::Info, "Hello Info");
    Ryu::Log(Ryu::LogLevel::Warn, "Hello Warn");
    Ryu::Log(Ryu::LogLevel::Error, "Hello Error");
    //Ryu::Log(Ryu::LogLevel::Fatal, "Hello Fatal");
}