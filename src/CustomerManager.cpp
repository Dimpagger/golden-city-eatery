#include <cstdlib>
#include <algorithm>
#include "CustomerManager.h"
#include "Constants.h"

void CustomerManager::ResetForDay(float dayMult) {
    customers.clear();
    lostCustomers = 0;
    gameTime = 0.0f;
    spawnTimer = 0.0f;
    dayMultiplier = dayMult;
    spawnInterval = SPAWN_INTERVAL_MIN * dayMult
                  + (rand() % (int)(SPAWN_INTERVAL_MAX * dayMult - SPAWN_INTERVAL_MIN * dayMult + 1));
}

float CustomerManager::GetDifficulty() const {
    return std::min(gameTime / DIFFICULTY_RAMP_TIME, 1.0f);
}

CustomerType CustomerManager::RollCustomerType() const {
    int total = SPAWN_WEIGHT_NORMAL + SPAWN_WEIGHT_IMPATIENT + SPAWN_WEIGHT_VIP;
    int roll = rand() % total;
    if (roll < SPAWN_WEIGHT_NORMAL) return CustomerType::NORMAL;
    roll -= SPAWN_WEIGHT_NORMAL;
    if (roll < SPAWN_WEIGHT_IMPATIENT) return CustomerType::IMPATIENT;
    return CustomerType::VIP;
}

float CustomerManager::PatienceForType(CustomerType type, float basePatience) const {
    switch (type) {
        case CustomerType::NORMAL:   return basePatience * PATIENCE_MULT_NORMAL;
        case CustomerType::IMPATIENT: return basePatience * PATIENCE_MULT_IMPATIENT;
        case CustomerType::VIP:      return basePatience * PATIENCE_MULT_VIP;
    }
    return basePatience;
}

void CustomerManager::Update(float dt) {
    gameTime += dt;
    float diff = GetDifficulty();

    // Spawn — base intervals scaled by day multiplier
    spawnTimer += dt;
    if (spawnTimer >= spawnInterval && (int)customers.size() < MAX_QUEUE) {
        int slot = (int)customers.size();
        float basePatience = (CUSTOMER_PATIENCE + (PATIENCE_MIN - CUSTOMER_PATIENCE) * diff) * dayMultiplier;
        CustomerType type = RollCustomerType();
        float patience = PatienceForType(type, basePatience);
        customers.push_back(std::make_unique<Customer>(QUEUE_X[slot], QUEUE_Y, type, patience));
        spawnTimer = 0.0f;

        float minInterval = (SPAWN_INTERVAL_MIN + (SPAWN_INTERVAL_MIN_FAST - SPAWN_INTERVAL_MIN) * diff) * dayMultiplier;
        float maxInterval = (SPAWN_INTERVAL_MAX + (SPAWN_INTERVAL_MAX_FAST - SPAWN_INTERVAL_MAX) * diff) * dayMultiplier;
        spawnInterval = minInterval + (rand() % (int)(maxInterval - minInterval + 1));
    }

    // Update
    for (auto& c : customers) {
        c->Update(dt);
    }

    // Remove left customers
    for (auto it = customers.begin(); it != customers.end(); ) {
        if ((*it)->HasLeft()) {
            if (!(*it)->WasServed()) {
                lostCustomers++;
            }
            it = customers.erase(it);
            for (int i = 0; i < (int)customers.size(); i++) {
                customers[i]->MoveTo(QUEUE_X[i], QUEUE_Y);
            }
        } else {
            ++it;
        }
    }
}

float CustomerManager::ServeFirstWaiting() {
    for (auto& c : customers) {
        if (c->IsWaiting()) {
            float mult = c->GetRewardMultiplier();
            c->Serve();
            return mult;
        }
    }
    return 0.0f;
}

const std::vector<std::unique_ptr<Customer>>& CustomerManager::GetCustomers() const {
    return customers;
}

int CustomerManager::GetQueueCount() const {
    return (int)customers.size();
}

int CustomerManager::GetLostCount() const {
    return lostCustomers;
}
