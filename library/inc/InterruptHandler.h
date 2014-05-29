
class InterruptHandler {

public:
    static InterruptHandler* getInstance(void);
private:
    InterruptHandler();
    ~InterruptHandler();

private:
    InterruptHandler * interruptHandler;
};
