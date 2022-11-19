#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "robot-spec.hpp"

#define DEBUG 0;

enum robotParsingStep{GET_ROBOT_COUNT, GET_ROBOT_ASSEMBLY_INFO, GET_ROBOT_NAME,
    GET_PART_ASSEMBLY_INSTRUCTIONS, GET_PART_ASSEMBLY_COSTS};

bool isStringWhitespace(std::string s);
int getTokenCount(std::string, char delim);

int main(int argc, char *argv[]) {
    if(argc <= 1){ //check if the user has provided a file name as an argument.
        std::cout << "Invalid Usage: You must provide a robot assembly file as a command line argument to this program." << std::endl;
        return 1;
    }
    std::ifstream inFile;
    int robotCount = 1;
    int currentRobotBeingParsed = 0;
    std::vector<RobotSpecification> robotList;
    std::string currentLine;
    inFile.open(argv[1], std::ios::in);
    if(!inFile){
        std::cout << "Could not open file " << argv[1] << "." << std::endl;
        return 1;
    }
    else{
        /*To parse a robot specification file, we need to work through (at most) 5 steps.
        * Step #1 is to determine how many robots are going to be defined, which is defined in the first line of the file
        * Step #2 is to get the name of the robot
        * Step #3 is to get the assembly info (how many parts and how many dependencies)
        * Step #4 is to read all of the assembly instructions and record it.
        * Step #5 is to read the costs to assemble each part.
        *
        * What step we are on while reading the file is stored in the currentStep variable, which is an enum
        * with the following possible values: GET_ROBOT_COUNT, GET_ROBOT_ASSEMBLY_INFO, GET_ROBOT_NAME,
        * GET_PART_ASSEMBLY_INSTRUCTIONS, GET_PART_ASSEMBLY_COSTS
        *
        * All of this data is stored in a RobotSpecification instance, which is a class defined in robot-spec.hpp
        * */
        robotParsingStep currentStep = GET_ROBOT_COUNT;
        robotType currentRobotType;
        int currentRobotPartCount = 0;
        int currentPartForAssemblyCostParsing = 0;
        int numberOfPartsForCurrentRobot = 0;
        int numberOfAssemblyDependencies = 0;
        int currentAssemblyInstruction = 0;
        int currentRobotamataNumberOfConstructionSteps = 0;
        int currentConstructionStepRobotamata = 0;
        for(std::string currentLine; std::getline(inFile, currentLine);){
            //This reads through the input file line-by-line, and loads the current line into the currentLine variable.
            if(DEBUG) std::cout << currentLine << std::endl;
            if(isStringWhitespace(currentLine)) continue; //We ignore whitespace lines.
            if(currentRobotBeingParsed >= robotCount) break;
            std::stringstream stringStream; //We instantiate a stringstream object with the current line as its contents,
                                            //since that will let us extract data as the right type from the string easier.
            stringStream << currentLine;
            switch(currentStep){
                case GET_ROBOT_COUNT:
                    //Step #1: Get the number of robots defined in the file
                    {
                        try{ // We try to read a single int from the current line
                            stringStream >> robotCount;
                        }
                        catch(std::exception& e){ //if it fails, we print a message and quit.
                            std::cout << "No robot count defined in " << argv[1] << "." <<std::endl;
                            return 2;
                        }
                        currentStep = GET_ROBOT_NAME; //We go onto the next step of parsing, getting the name.
                        for(int i = 0; i < robotCount; i++){
                            robotList.push_back(RobotSpecification()); //we instantiate all of the robots needed.
                        }
                    }
                    break;
                case GET_ROBOT_NAME:
                    {
                        //Step #2: We need to get the name of the robot from the current line.
                        std::string currentRobotName;
                        try{
                            stringStream >> currentRobotName;
                        }
                        catch(std::exception& e){
                            std::cout << "No name for robot #" << currentRobotBeingParsed << std::endl;
                            return 3;
                        }
                        robotList[currentRobotBeingParsed].setName(currentRobotName);
                        currentStep = GET_ROBOT_ASSEMBLY_INFO;
                    }
                    break;
                case GET_ROBOT_ASSEMBLY_INFO:
                    {
                        //Step #3 is to get the assembly info, the number of parts needed and the number of assembly dependencies.
                        int numAssemblyDependencies;
                        int numParts;
                        int numberOfStagesInConstruction;
                        int numberOfTokens = getTokenCount(currentLine, ' '); //We want to know how many tokens are being read, this tells us if the input is malformed as well as if it's a robotamata or omnibot
                        try{
                            switch(numberOfTokens){
                            case 2:
                                stringStream >> numParts >> numAssemblyDependencies; //if there's two tokens in the string, we read both as the number of parts and assembly deps respectively.
                                currentRobotPartCount = numParts; // We want to know how mnay robot parts there are so that when we're getting the part costs we know when to jump to the next step.
                                numberOfAssemblyDependencies = numAssemblyDependencies;
                                robotList[currentRobotBeingParsed].setNumberOfParts(numParts);
                                robotList[currentRobotBeingParsed].setNumberOfAssemblyDependencies(numAssemblyDependencies);
                                currentAssemblyInstruction = 0;
                                currentRobotType = OMNIDROID;
                                break;
                            case 1:
                                //TODO: robotamata stuff
                                stringStream >> numberOfStagesInConstruction;
                                currentRobotamataNumberOfConstructionSteps = numberOfStagesInConstruction;
                                currentConstructionStepRobotamata = 0;
                                currentRobotType = ROBOTAMATON;
                                robotList[currentRobotBeingParsed].setRobotType(ROBOTAMATON);
                                robotList[currentRobotBeingParsed].setNumberOfConstructionStages(numberOfStagesInConstruction);
                                break;
                            default:
                                std::cout << "Incorrect number of arguments robot spec info. (first line after name of robot)" << std::endl;
                                return 7;
                            }
                        }
                        catch(std::exception& e){
                            std::cout << "Could not read assembly instructions." << std::endl;
                            return 6;
                        }
                        currentStep = GET_PART_ASSEMBLY_INSTRUCTIONS;
                    }
                    break;
                case GET_PART_ASSEMBLY_INSTRUCTIONS:
                    {
                        //Step 4: We now need to record all of the assembly instructions
                        part partNeeded; //An assembly instruction has what part is need and what that part is used to assemble.
                        part partToAssemble;
                        cost costOfCurrentPart;
                        numPreviousStages numPrevStages;
                        int tokenCount = getTokenCount(currentLine, ' '); //We get the token count because after reading the last
                                                                          //instruction the next line will be the first line of assembly costs
                                                                          //so if there is only one token we know we're actually in Step#4 now.
                        try{
                            //there's two tokens at least so read them in as the parts needed and part to assemble respectively.
                            if(currentRobotType == OMNIDROID){
                                stringStream >> partNeeded >> partToAssemble;
                                robotList[currentRobotBeingParsed].addAssemblyInstruction(partNeeded, partToAssemble);
                                currentAssemblyInstruction++;
                            }
                            else{
                                stringStream >> costOfCurrentPart >> numPrevStages;
                                robotList[currentRobotBeingParsed].pushRobotamataStep(costOfCurrentPart, numPrevStages);
                                currentConstructionStepRobotamata++;
                            }
                        }
                        catch(std::exception& e){
                            std::cout << "Couldn't parse assembly instructions for robot " <<
                                currentRobotBeingParsed << "!" <<std::endl;
                            return 4;
                        }
                        if(currentRobotType == OMNIDROID && currentAssemblyInstruction >= numberOfAssemblyDependencies){
                            currentStep = GET_PART_ASSEMBLY_COSTS;
                            currentAssemblyInstruction = 0;
                            numberOfAssemblyDependencies = 0;
                        }
                        else if(currentRobotType == ROBOTAMATON && currentConstructionStepRobotamata >= currentRobotamataNumberOfConstructionSteps){
                            currentStep = GET_ROBOT_NAME;
                            currentConstructionStepRobotamata = 0;
                            currentRobotamataNumberOfConstructionSteps = 0;
                        }
                    }
                    break;
                case GET_PART_ASSEMBLY_COSTS:
                    {
                        //Step 5: We're now getting the assembly costs for the robot.
                        cost val;
                        try {stringStream >> val;}
                        catch(std::exception& e){
                            std::cout << "Could not parse assembly cost for part "
                                << currentPartForAssemblyCostParsing
                                << " for robot# " << currentRobotBeingParsed << std::endl;
                            return 5;
                        }
                        robotList[currentRobotBeingParsed].addAssemblyCost(val);
                        currentPartForAssemblyCostParsing++;
                        if(currentPartForAssemblyCostParsing >= currentRobotPartCount){
                            //if we've now parsed every part's cost we now jump to reading the name of the next robot.
                            currentStep = GET_ROBOT_NAME;

                            robotList[currentRobotBeingParsed].print();
                            currentRobotBeingParsed++;
                            currentPartForAssemblyCostParsing = 0; //Reset variables used for part counting
                            currentRobotPartCount = 0;
                        }
                    }
                    break;
            }
        }
        return 0;
    }
}

bool isStringWhitespace(std::string s){
    for(auto it = s.begin(); it != s.end(); it++){
        if(!std::isspace(*it)) return false;
    }
    return true;
}

int getTokenCount(std::string s, char delim){
    std::stringstream ss(s);
    std::string temp;
    int tokenCount = 0;
    while(std::getline(ss, temp, delim)){
        tokenCount++;
    }
    return tokenCount;
}
