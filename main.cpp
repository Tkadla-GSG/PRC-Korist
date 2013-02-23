/* 
 * File:   main.cpp
 * Author: Tkadla
 *
 * Created on 22. únor 2013, 18:57
 */

#include <cstdlib>
#include <cstring> 
#include <iostream> 
#include <fstream>
#include <stack>
#include <math.h>
#include <limits>

using namespace std;

/**
 * GLOBAL VARS
 */
int * prices;
int thiefs;
int things;
int loot;

/**
 * Debug - array printer
 * @param array
 * @param length
 */
void array_dump(int * array, int length) {

    for (int i = 0; i < length; i++) {
        cout << array[i] << " | ";
    }

    cout << endl;
}

/**
 * Stav
 */
class State {
public:

    /**
     * Constructor
     * @param bags      - array with current prices in bags
     * @param level     - how many things have been already added to the bags
     */
    State(int * bags, int level) {
        this->bags = bags;
        this->level = level;
        this->fairness = 0.0; 
    }

    /**
     * Destructor
     */
    ~State() {
        delete this->bags;
        bags = NULL;
    }

private:
    int * bags;
    int level;
    double fairness;
    
    /**
     * Calculate fairness of this state and returns it as absolute value
     * @return 
     */
    void solve() {

        double max = 0.0;
        for (int i = 0; i < thiefs; i++) {
            double value = fabs(this->bags[i] - (loot / thiefs));
            //cout << "fairness " << i << " |  " << value << endl;
            if (value > max) {
                max = value;
            }
        }

        this->fairness = max;
        
        // DEBUG
        this->dumpState();
    }

public:

    /**
     * Dump all children of this state to stack
     * @param mainStack
     */
    void getChildren(stack < State * > & mainStack) {

        //generate nothing if all things has been already distributed
        if (level >= things) {
            return;
        }

        //every state will have excatly 'thiefs' children
        for (int i = 0; i < thiefs; i++) {

            // Create fresh copy of bags array
            int * newBags = new int[thiefs];
            for (int j = 0; j < thiefs; j++) {
                newBags[j] = bags[j];
            }

            // dispance thing into one bag
            newBags[i] += prices[level];

            State * newState = new State(newBags, level + 1);
            newState->solve(); 
            
            mainStack.push(newState);

            //DEBUG
            //cout << "new state generated" << endl;
            //newState->dumpState();
        }
    }
    
    double getFairness(){
        return this->fairness; 
    }

    /**
     * DEBUG - print all info about state
     */
    void dumpState() {

        cout << "///////State Dump///////" << endl;
        array_dump(bags, thiefs);
        cout << "level: " << level << endl;
        cout << "fairness: " << fairness << endl;
        cout << "////////////////////////" << endl << endl;
        ;

    }

};

State * work(stack < State * > & mainStack) {

    double min = numeric_limits<double>::max();
    State * minState = NULL;
    State * current = NULL;
    double currentValue = 0; 

    while (!mainStack.empty()) {
        current = mainStack.top();
        mainStack.pop();

        //get fairness of current state
        currentValue = current->getFairness();
        //push all its childrens into stack
        current->getChildren(mainStack);

        // <= je tady proto, protoze takhle chytnu nejvic fair rozdeleni NEJVIC predmetu
        if (currentValue <= min) {
            
            if (minState != NULL) {
                delete minState;
            }

            min = currentValue; 
            minState = current;

        } else {

            delete current;

        }

    }

    return minState;
}

/*
 * Entry point 
 */
int main(int argc, char** argv) {
    // Nahrani souboru 
    ifstream file;
    string fileName = "vstup.txt";
    file.open(fileName.c_str());
    if (!file) {
        cout << "Error in openening file" << endl;
        return EXIT_FAILURE;
    }

    string line;
    // Prvni radek je pocet zlodeju 
    getline(file, line);
    thiefs = atoi(line.c_str());

    // Druhy radek je pocet kusu lupu
    getline(file, line);
    things = atoi(line.c_str());

    // jednicky v radku generatoru grafu sectu a dostanu tak cenu jedne veci
    prices = new int [things];

    for (int j = 0; j < things; j++) {

        getline(file, line);
        int price = 0;

        // secti vsechno v radku 
        for (int i = 0; i < things; i++) {
            char ch = line.at(i);
            price += atoi(&ch);
        }

        prices[j] = price;
    }

    //work out loot value
    for (int i = 0; i < things; i++) {
        loot += prices[i];
    }

    // Stavovy zasobnik
    stack < State * > mainStack;

    // create and reset array of prices in all bags (one for every thief)
    int * bags = new int[thiefs];
    for (int i = 0; i < thiefs; i++) {
        bags[i] = 0;
    }

    State * init = new State(bags, 0);

    mainStack.push(init);

    State * result = work(mainStack);

    cout << "///////// RESULT //////////" << endl;
    result->dumpState();
    cout << "/////////////////////////////" << endl;

    
    // cleanup
    delete result; 
    result = NULL; 
    
    delete prices;
    prices = NULL; 
   
    State * current = NULL;
    while (!mainStack.empty()) {
        current = mainStack.top();
        mainStack.pop();
        delete current;
    }

    return 0;
}

