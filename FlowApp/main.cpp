#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

class Step {
public:
    virtual void execute() = 0;
};

class TitleStep : public Step {
private:
    string title;
    string subtitle;

public:
    TitleStep(const string& t, const string& st) : title(t), subtitle(st) {}

    void execute() override {
        cout << "Title: " << title << endl;
        cout << "Subtitle: " << subtitle << endl;
    }
};

class TextStep : public Step {
private:
    string title;
    string copy;

public:
    TextStep(const string& t, const string& c) : title(t), copy(c) {}

    void execute() override {
        cout << "Title: " << title << endl;
        cout << "Copy: " << copy << endl;
    }
};

// Define other step classes similarly

// Class for Flow
class Flow {
private:
    string name;
    vector<Step*> steps;
    time_t creationTimestamp;

public:
    Flow(const string& n) : name(n), creationTimestamp(time(0)) {}

    void addStep(Step* step) {
        steps.push_back(step);
    }

    void run() {
        for (Step* step : steps) {
            step->execute();
        }
    }

    // Add methods for analytics and other flow-related functionalities
};

// Class for System
class System {
private:
    vector<Flow*> flows;

public:
    void createFlow(const string& name) {
        flows.push_back(new Flow(name));
    }

    void deleteFlow(Flow* flow) {
        // Implement flow deletion
    }

    // Add other methods for analytics, error handling, etc.
};

int main() {
    System mySystem;

    // Creating a flow and adding steps
    Flow* myFlow = new Flow("MyFirstFlow");
    myFlow->addStep(new TitleStep("Main Title", "Subtitle"));
    myFlow->addStep(new TextStep("Text Step", "This is a text step."));

    // Add more steps to the flow as needed

    mySystem.createFlow("MyFirstFlow");

    // Running the flow
    myFlow->run();

    // Cleaning up resources
    mySystem.deleteFlow(myFlow);

    return 0;
}
