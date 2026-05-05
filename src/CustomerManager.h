#pragma once

#include <memory>
#include <vector>
#include "Customer.h"

class CustomerManager {
public:
    void ResetForDay(float dayMult);
    void Update(float dt);
    float ServeFirstWaiting();  // returns reward multiplier, 0 if none
    const Customer* GetFirstWaiting() const;  // for recipe matching

    const std::vector<std::unique_ptr<Customer>>& GetCustomers() const;
    int GetQueueCount() const;
    int GetLostCount() const;
    float GetLastLostX() const;
    float GetLastLostY() const;

private:
    float GetDifficulty() const;
    CustomerType RollCustomerType() const;
    RecipeType RollRecipeType() const;
    float PatienceForType(CustomerType type, float basePatience) const;

    std::vector<std::unique_ptr<Customer>> customers;
    float spawnTimer = 0.0f;
    float spawnInterval = 0.0f;
    float gameTime = 0.0f;
    float dayMultiplier = 1.0f;
    int lostCustomers = 0;
    float lastLostX = 0.0f;
    float lastLostY = 0.0f;
};
