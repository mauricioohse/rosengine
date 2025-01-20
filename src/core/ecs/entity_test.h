#ifndef ENTITY_TEST_H
#define ENTITY_TEST_H

#include "entity.h"
#include <stdio.h>

inline void TestEntityManager(){
    printf("\n=== Testing Entity Manager ===\n");
    
    EntityManager manager;
    manager.Init();
    
    // Test 1: Create entities
    printf("\nTest 1: Creating entities\n");
    EntityID entity1 = manager.CreateEntity();
    EntityID entity2 = manager.CreateEntity();
    EntityID entity3 = manager.CreateEntity();
    
    printf("Created entities: %u, %u, %u\n", entity1, entity2, entity3);
    printf("Entity count: %u\n", manager.entityCount);
    
    // Test 2: Check entity validity
    printf("\nTest 2: Checking entity validity\n");
    printf("Entity %u valid: %s\n", entity1, manager.IsEntityValid(entity1) ? "yes" : "no");
    printf("Entity %u valid: %s\n", entity2, manager.IsEntityValid(entity2) ? "yes" : "no");
    printf("Entity 0 valid: %s\n", manager.IsEntityValid(0) ? "yes" : "no");
    
    // Test 3: Destroy entity
    printf("\nTest 3: Destroying entity\n");
    printf("Before destruction - Entity count: %u\n", manager.entityCount);
    manager.DestroyEntity(entity2);
    printf("After destroying entity %u - Entity count: %u\n", entity2, manager.entityCount);
    printf("Entity %u valid: %s\n", entity2, manager.IsEntityValid(entity2) ? "yes" : "no");
    
    // Test 4: Create entity after destruction
    printf("\nTest 4: Creating new entity after destruction\n");
    EntityID entity4 = manager.CreateEntity();
    printf("New entity created: %u\n", entity4);
    printf("Entity count: %u\n", manager.entityCount);
    
    // Test 5: Debug print active entities
    printf("\nTest 5: Active entities status:\n");
    for (EntityID i = 0; i < 5; i++) {
        printf("Entity %u: active=%d, mask=%u\n", 
               i, 
               manager.activeEntities[i], 
               manager.componentMasks[i]);
    }
}
#endif // ENTITY_TEST_H 