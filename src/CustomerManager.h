#pragma once

#include <memory>
#include <vector>
#include "Customer.h"

class CustomerManager {
public:
    void ResetForDay(float dayMult);
    void Update(float dt);
    float ServeFirstWaiting();  // returns reward multiplier, 0 if none

    const std::vector<std::unique_ptr<Customer>>& GetCustomers() const;
    int GetQueueCount() const;
    int GetLostCount() const;

private:
    float GetDifficulty() const;
    CustomerType RollCustomerType() const;
    float PatienceForType(CustomerType type, float basePatience) const;

    std::vector<std::unique_ptr<Customer>> customers;
    float spawnTimer = 0.0f;
    float spawnInterval = 0.0f;
    float gameTime = 0.0f;
    float dayMultiplier = 1.0f;
    int lostCustomers = 0;
};
