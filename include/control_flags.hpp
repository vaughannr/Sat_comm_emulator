
class ControlFlags {
public:
    ControlFlags() : closeSubscriberLoop(false), closeControlLoop(false) {}
    void closeAll(){ closeSubscriberLoop = true; closeControlLoop = true; };
    void resetAll(){ closeSubscriberLoop = false; closeControlLoop = false; };

    bool closeSubscriberLoop;
    bool closeControlLoop;
};