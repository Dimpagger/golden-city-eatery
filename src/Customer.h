#pragma once

class Customer {
public:
    enum class State { ENTERING, WAITING, LEAVING };

    // targetX/targetY: where to stand in the queue
    Customer(float targetX, float targetY);

    void Update(float dt);

    void Serve();            // called when food is delivered
    bool HasLeft() const;    // true when fully off-screen or patience ran out
    bool IsWaiting() const;
    bool WasServed() const;  // left with food (happy)

    State GetState() const;
    float GetX() const;
    float GetY() const;
    float GetPatience() const;

    void MoveTo(float newTargetX, float newTargetY);  // shift forward in queue

private:
    float x, y;
    float targetX, targetY;
    float patience = 30.0f;
    float moveSpeed = 150.0f;
    State state = State::ENTERING;
    bool served = false;

    static constexpr float PATIENCE_MAX = 30.0f;
};
