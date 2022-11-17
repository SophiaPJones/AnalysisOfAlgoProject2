#ifndef ROBOT-SPEC_H_
#define ROBOT-SPEC_H_
#include <vector>
#include <tuple>
#include <string>
#include <map>

using part = int;
using cost = int;
class RobotSpecification{
private:
    std::string robotName;
    std::map<part, std::vector<part>> assemblySpecification;
    std::vector<cost> partAssemblySprocketCosts;
public:
    RobotSpecification();
    ~RobotSpecification();
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
    };
    void addAssemblyCost(cost c){
        partAssemblySprocketCosts.push_back(c);
    }
    cost getAssemblyCost(part whichPart){
        return partAssemblySprocketCosts.at(whichPart);
    }
    cost getTotalCostForPart(part whichPart); //recursive function to get total cost
    int getPartCount(){
        return assemblySpecification.size();
    }
};
#endif // ROBOT-SPEC_H_
