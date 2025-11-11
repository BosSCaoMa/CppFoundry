#include "../include/memory/unique_ptr.hpp"
#include "../include/memory/shared_ptr.hpp"
#include "../include/containers/vector.hpp"
#include "../include/containers/list.hpp"
#include <iostream>
#include <string>

using namespace cppfoundry;

// Example class for demonstrations
class Person {
public:
    std::string name;
    int age;
    
    Person(const std::string& n, int a) : name(n), age(a) {
        std::cout << "Person created: " << name << ", age " << age << std::endl;
    }
    
    ~Person() {
        std::cout << "Person destroyed: " << name << std::endl;
    }
    
    void introduce() const {
        std::cout << "Hi, I'm " << name << " and I'm " << age << " years old." << std::endl;
    }
};

void unique_ptr_examples() {
    std::cout << "\n=== unique_ptr Examples ===" << std::endl;
    
    // Basic usage
    {
        std::cout << "\n1. Basic unique_ptr usage:" << std::endl;
        unique_ptr<Person> p1(new Person("Alice", 25));
        p1->introduce();
    }  // p1 automatically deleted here
    
    // Using make_unique
    {
        std::cout << "\n2. Using make_unique (recommended):" << std::endl;
        auto p2 = make_unique<Person>("Bob", 30);
        p2->introduce();
    }
    
    // Move semantics
    {
        std::cout << "\n3. Move semantics:" << std::endl;
        auto p3 = make_unique<Person>("Charlie", 35);
        std::cout << "Before move: p3 is " << (p3 ? "valid" : "null") << std::endl;
        
        auto p4 = std::move(p3);  // Transfer ownership
        std::cout << "After move: p3 is " << (p3 ? "valid" : "null") << std::endl;
        std::cout << "After move: p4 is " << (p4 ? "valid" : "null") << std::endl;
        p4->introduce();
    }
    
    // Array version
    {
        std::cout << "\n4. unique_ptr with arrays:" << std::endl;
        unique_ptr<int[]> arr(new int[5]);
        for (int i = 0; i < 5; ++i) {
            arr[i] = i * 10;
        }
        std::cout << "Array elements: ";
        for (int i = 0; i < 5; ++i) {
            std::cout << arr[i] << " ";
        }
        std::cout << std::endl;
    }
}

void shared_ptr_examples() {
    std::cout << "\n=== shared_ptr Examples ===" << std::endl;
    
    // Basic usage
    {
        std::cout << "\n1. Basic shared_ptr usage:" << std::endl;
        shared_ptr<Person> p1(new Person("Diana", 28));
        std::cout << "Reference count: " << p1.use_count() << std::endl;
        p1->introduce();
    }
    
    // Shared ownership
    {
        std::cout << "\n2. Shared ownership:" << std::endl;
        auto p2 = make_shared<Person>("Eve", 32);
        std::cout << "After creation, ref count: " << p2.use_count() << std::endl;
        
        {
            shared_ptr<Person> p3 = p2;  // Share ownership
            std::cout << "After copy, ref count: " << p2.use_count() << std::endl;
            std::cout << "p3 ref count: " << p3.use_count() << std::endl;
        }
        
        std::cout << "After p3 destroyed, ref count: " << p2.use_count() << std::endl;
    }  // Person destroyed here when last reference goes out of scope
    
    // Multiple owners
    {
        std::cout << "\n3. Multiple owners scenario:" << std::endl;
        auto manager = make_shared<Person>("Manager Frank", 45);
        
        vector<shared_ptr<Person>> team;
        team.push_back(manager);
        team.push_back(manager);
        team.push_back(manager);
        
        std::cout << "Manager shared by " << manager.use_count() << " references" << std::endl;
    }
}

void vector_examples() {
    std::cout << "\n=== vector Examples ===" << std::endl;
    
    // Basic operations
    {
        std::cout << "\n1. Basic vector operations:" << std::endl;
        vector<int> v;
        
        // Add elements
        for (int i = 1; i <= 5; ++i) {
            v.push_back(i * 10);
        }
        
        std::cout << "Vector size: " << v.size() << std::endl;
        std::cout << "Vector capacity: " << v.capacity() << std::endl;
        
        std::cout << "Elements: ";
        for (size_t i = 0; i < v.size(); ++i) {
            std::cout << v[i] << " ";
        }
        std::cout << std::endl;
    }
    
    // Initializer list
    {
        std::cout << "\n2. Vector from initializer list:" << std::endl;
        vector<std::string> names = {"Alice", "Bob", "Charlie", "Diana"};
        
        std::cout << "Names: ";
        for (const auto& name : names) {
            std::cout << name << " ";
        }
        std::cout << std::endl;
    }
    
    // With smart pointers
    {
        std::cout << "\n3. Vector of smart pointers:" << std::endl;
        vector<shared_ptr<Person>> people;
        
        people.push_back(make_shared<Person>("George", 40));
        people.push_back(make_shared<Person>("Helen", 38));
        
        std::cout << "People in vector:" << std::endl;
        for (const auto& person : people) {
            person->introduce();
        }
    }  // All Person objects automatically cleaned up
}

void list_examples() {
    std::cout << "\n=== list Examples ===" << std::endl;
    
    // Basic operations
    {
        std::cout << "\n1. Basic list operations:" << std::endl;
        list<int> l;
        
        // Add to front and back
        l.push_back(3);
        l.push_back(4);
        l.push_front(2);
        l.push_front(1);
        l.push_back(5);
        
        std::cout << "List elements: ";
        for (const auto& val : l) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
    
    // Insert and erase
    {
        std::cout << "\n2. Insert and erase operations:" << std::endl;
        list<std::string> tasks = {"Task 1", "Task 2", "Task 4"};
        
        // Insert in middle
        auto it = tasks.begin();
        ++it;
        ++it;
        tasks.insert(it, "Task 3");
        
        std::cout << "Tasks after insert:" << std::endl;
        for (const auto& task : tasks) {
            std::cout << "  - " << task << std::endl;
        }
        
        // Remove first task
        tasks.pop_front();
        std::cout << "After removing first task:" << std::endl;
        for (const auto& task : tasks) {
            std::cout << "  - " << task << std::endl;
        }
    }
    
    // Bidirectional iteration
    {
        std::cout << "\n3. Bidirectional iteration:" << std::endl;
        list<int> l = {1, 2, 3, 4, 5};
        
        auto it = l.end();
        std::cout << "Reverse order: ";
        while (it != l.begin()) {
            --it;
            std::cout << *it << " ";
        }
        std::cout << std::endl;
    }
}

void combined_example() {
    std::cout << "\n=== Combined Example: Task Management System ===" << std::endl;
    
    struct Task {
        std::string description;
        int priority;
        
        Task(const std::string& desc, int prio) 
            : description(desc), priority(prio) {}
    };
    
    // Vector of shared pointers to tasks
    vector<shared_ptr<Task>> high_priority_tasks;
    list<shared_ptr<Task>> low_priority_tasks;
    
    // Create tasks
    auto task1 = make_shared<Task>("Fix critical bug", 10);
    auto task2 = make_shared<Task>("Write documentation", 3);
    auto task3 = make_shared<Task>("Review code", 7);
    auto task4 = make_shared<Task>("Update tests", 5);
    
    // Organize by priority
    if (task1->priority >= 7) high_priority_tasks.push_back(task1);
    if (task2->priority < 7) low_priority_tasks.push_back(task2);
    if (task3->priority >= 7) high_priority_tasks.push_back(task3);
    if (task4->priority < 7) low_priority_tasks.push_back(task4);
    
    // Display
    std::cout << "\nHigh Priority Tasks:" << std::endl;
    for (const auto& task : high_priority_tasks) {
        std::cout << "  [" << task->priority << "] " << task->description << std::endl;
    }
    
    std::cout << "\nLow Priority Tasks:" << std::endl;
    for (const auto& task : low_priority_tasks) {
        std::cout << "  [" << task->priority << "] " << task->description << std::endl;
    }
    
    std::cout << "\nNote: All tasks will be automatically cleaned up when going out of scope!" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  CppFoundry - STL Components Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        unique_ptr_examples();
        shared_ptr_examples();
        vector_examples();
        list_examples();
        combined_example();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "  Demo completed successfully!" << std::endl;
        std::cout << "========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
