#ifndef ROBOT-SPEC_H_
#define ROBOT-SPEC_H_
#include <vector>
#include <tuple>
#include <string>
#include <map>
#include <stack>

using part = int;
using cost = int;
using numPreviousStages = int;
enum robotType{OMNIDROID, ROBOTOMATON};
class RobotSpecification{
private:
    std::string robotName;
    robotType type;
    int numberOfStagesInConstruction;
    std::vector<std::pair<cost, numPreviousStages>> robotomatonSpecification;
    std::map<part, std::vector<part>> assemblySpecification;
    std::vector<cost> partAssemblySprocketCosts;
    int numberOfParts;
    int numberOfAssemblyDependencies;
public:
    RobotSpecification();
    ~RobotSpecification();
    void pushRobotomatonStep(cost costVal, numPreviousStages numPrevStages){
        this->robotomatonSpecification.push_back(std::pair<cost, numPreviousStages>(costVal, numPrevStages));
    }
    std::vector<std::pair<cost, numPreviousStages>> getRobotomatonSteps(){
        return this->robotomatonSpecification;
    }
    robotType getRobotType(){return type;}
    void setRobotType(robotType type){this->type = type;}
    void setNumberOfConstructionStages(int number){this->numberOfStagesInConstruction = number;}
    int getNumberOfConstructionStages(){return this->numberOfStagesInConstruction;}
    int getNumberOfParts(){return numberOfParts;}
    int getNumberOfAssemblyDependencies(){return this->numberOfAssemblyDependencies;}
    void setNumberOfParts(int numberOfParts){this->numberOfParts = numberOfParts;}
    void setNumberOfAssemblyDependencies(int numberOfAssemblyDependencies){
        this->numberOfAssemblyDependencies = numberOfAssemblyDependencies;}
    std::string getName(){ return robotName; }
    void setName(std::string name) { this->robotName = name; }
    std::vector<part> getRequiredPartsToAssemblePart(part partToAssemble){
        return assemblySpecification[partToAssemble];
    };
    void addAssemblyInstruction(part requirementPart, part assembledPart){
        auto iteratorToPartInMap = assemblySpecification.find(assembledPart);
        if(iteratorToPartInMap == assemblySpecification.end()){
            //part not found, insert new keyval pair
            std::vector<part> newRequirementList;
            newRequirementList.push_back(requirementPart);
            assemblySpecification.insert(std::pair<part,std::vector<part>>(
                assembledPart, newRequirementList));
        }
        else{
            std::vector<part> *requirementList = &(iteratorToPartInMap->second);
            requirementList->push_back(requirementPart);
        }
        return;
    };
    void addAssemblyCost(cost c){
        this->partAssemblySprocketCosts.push_back(c);
        return;
    }
    cost getAssemblyCost(part whichPart){
        return this->partAssemblySprocketCosts.at(whichPart);
    }
    cost getTotalCostForPart(part whichPart); //recursive function to get total cost
    int getPartCount(){
        return this->numberOfParts;

    }
    void print();
};
#endif // ROBOT-SPEC_H_
