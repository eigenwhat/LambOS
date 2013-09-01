class TerminalOutputStream : public OutputStream
{
public:
    TerminalOutputStream(Terminal &term) : _terminal(term) {}
    virtual void write(uint8_t byte) { _terminal.putChar((char)byte); }
private:
    Terminal &_terminal;    
};
