void main()
{
    // Print a message
    Ryu::Log(Ryu::LogLevel::Trace, "Hello from AS - Trace");
    Ryu::Log(Ryu::LogLevel::Debug, "Hello from AS - Debug");
    Ryu::Log(Ryu::LogLevel::Info, "Hello from AS - Info");
    Ryu::Log(Ryu::LogLevel::Warn, "Hello from AS - Warn");
    Ryu::Log(Ryu::LogLevel::Error, "Hello from AS - Error");
    //Ryu::Log(Ryu::LogLevel::Fatal, "Hello from AS - Fatal");
}
