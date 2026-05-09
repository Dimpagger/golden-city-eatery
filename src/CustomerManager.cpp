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
    int range = (int)((SPAWN_INTERVAL_MAX - SPAWN_INTERVAL_MIN) * dayMult);
    if (range < 0) range = 0;
    spawnInterval = SPAWN_INTERVAL_MIN * dayMult + (rand() % (range + 1));
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
        RecipeType desired = RollRecipeType();
        customers.push_back(std::make_unique<Customer>(QUEUE_X[slot], QUEUE_Y, type, patience, desired));
        spawnTimer = 0.0f;

        float minInterval = (SPAWN_INTERVAL_MIN + (SPAWN_INTERVAL_MIN_FAST - SPAWN_INTERVAL_MIN) * diff) * dayMultiplier;
        float maxInterval = (SPAWN_INTERVAL_MAX + (SPAWN_INTERVAL_MAX_FAST - SPAWN_INTERVAL_MAX) * diff) * dayMultiplier;
        int sRange = (int)(maxInterval - minInterval);
        if (sRange < 0) sRange = 0;
        spawnInterval = minInterval + (rand() % (sRange + 1));
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
                lastLostX = (*it)->GetX();
                lastLostY = (*it)->GetY();
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

const Customer* CustomerManager::GetFirstWaiting() const {
    for (auto& c : customers) {
        if (c->IsWaiting()) return c.get();
    }
    return nullptr;
}

void CustomerManager::SetAvailableRecipes(bool liangpi, bool kebab) {
    availLiangpi = liangpi;
    availKebab = kebab;
}

RecipeType CustomerManager::RollRecipeType() const {
    // Only roll among recipes the player has discovered
    int count = 1 + (availLiangpi ? 1 : 0) + (availKebab ? 1 : 0);
    int roll = rand() % count;
    if (roll == 0) return RecipeType::ROUJIAMO;
    if (availLiangpi && roll == 1) return RecipeType::LIANGPI;
    return RecipeType::KEBAB;
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

float CustomerManager::GetLastLostX() const { return lastLostX; }
float CustomerManager::GetLastLostY() const { return lastLostY; }
