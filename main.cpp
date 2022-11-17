#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "robot-spec.hpp"

enum robotParsingStep{GET_ROBOT_COUNT, GET_ROBOT_NAME,
    GET_PART_ASSEMBLY_INSTRUCTIONS, GET_PART_ASSEMBLY_COSTS};

bool isStringWhitespace(std::string s);
int getTokenCount(std::string, char delim);

int main(int argc, char *argv[]) {
    std::cout << "argc: " << argc << "\targv[1]:" << argv[1] << std::endl;
    if(argc <= 1){
        std::cout << "Invalid Usage: You must provide a robot assembly file as a command line argument to this program." << std::endl;
        return 1; //
    }
    std::ifstream inFile;
    int robotCount = 0;
    int currentRobotBeingParsed = 0;
    std::vector<RobotSpecification> robotList;
    std::string currentLine;
    inFile.open(argv[1], std::ios::in);
    if(!inFile){
        std::cout << "Could not open file " << argv[1] << "." << std::endl;
        return 1;
    }
    else{
        robotParsingStep currentStep = GET_ROBOT_COUNT;
        int currentRobotPartCount = 0;
        int currentPartForAssemblyCostParsing = 0;
        for(std::string currentLine; std::getline(inFile, currentLine);){
            std::cout << currentLine << std::endl;
            if(isStringWhitespace(currentLine)) continue;
            std::stringstream stringStream;
            stringStream << currentLine;
            switch(currentStep){
                case GET_ROBOT_COUNT:
                    try{
                        stringStream >> robotCount;
                    }
                    catch(std::exception& e){
                        std::cout << "No robot count defined in " << argv[1] << "." <<std::endl;
                        return 2;
                    }
                    currentStep = GET_ROBOT_NAME;
                    for(int i = 0; i < robotCount; i++){
                        robotList.push_back(RobotSpecification());
                    }
                    break;
                case GET_ROBOT_NAME:
                    std::string currentRobotName;
                    try{
                        stringStream >> currentRobotName;
                    }
                    catch(std::exception& e){
                        std::cout << "No name for robot #" << currentRobotBeingParsed << std:endl;
                        return 3;
                    }
                    robotList.at(currentRobotBeingParsed).setName(currentRobotName);
                    currentStep = GET_PART_ASSEMBLY_INSTRUCTIONS;
                    break;
                case GET_PART_ASSEMBLY_INSTRUCTIONS:
                    part partNeeded;
                    part partToAssemble;
                    int tokenCount = getTokenCount(currentLine, ' ');
                    try{
                        if(tokenCount == 1){
                            //we're looking at an assembly cost because there's only 1 token
                            cost val;
                            stringStream >> val;
                            robotList.at(currentRobotBeingParsed).addAssemblyCost(val);
                            currentRobotPartCount = robotList.at(currentRobotBeingParsed).getPartCount();
                            currentPartForAssemblyCostParsing = 1;
                            if(currentRobotPartCount <= currentPartForAssemblyCostParsing){
                                currentStep = GET_ROBOT_NAME;
                                break;
                            }
                            currentStep = GET_PART_ASSEMBLY_COSTS;
                            break;
                        }
                        stringStream >> partNeeded >> partToAssemble;
                    }
                    catch(std::exception& e){
                        std::cout << "Couldn't parse assembly instructions for robot " <<
                            currentRobotBeingParsed << "!" <<std::endl;
                        return 4;
                    }
                    robotList.at(currentRobotBeingParsed).addAssemblyInstruction(partNeeded, partToAssemble);
                    break;
                case GET_PART_ASSEMBLY_COSTS:
                    cost val;
                    try {stringStream >> val;}
                    catch(std::exception& e){
                        std::cout << "Could not parse assembly cost for part "
                            << currentPartForAssemblyCostParsing
                            << " for robot# " << currentRobotBeingParsed << std::endl;
                        return 5;
                    }
                    robotList.at(currentRobotBeingParsed).addAssemblyCost(val);
                    currentPartForAssemblyCostParsing++;
                    if(currentPartForAssemblyCostParsing >= currentRobotPartCount){
                        currentStep = GET_ROBOT_NAME;
                        currentPartForAssemblyCostParsing = 0;
                        break;
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
