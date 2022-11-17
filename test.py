# add single digits at end of file to a map (key 0 = 4, key 1 = 2, etc)
# Go through double number commands (x, y) add map[x] to map[y] (map[1] = map[1] + map[0] for the first command in this example
# Return the last entry in the maps’ value ie map[7] for this example

#open file
f = open('input.txt', 'r')

#Get the first line of the file (the type of robot)
line = f.readline()
line = line.strip()         #removes whitespace and newline characters from the beginning and end of the string (but not middle)
if line == 'omnidroid':
        print('THIS LINE IS THE TYPE OF ROBOT: ' + line)


#get the second line of the file (the number of parts and how many steps are involved in building the robot)
line = f.readline()

#now we have a string in the form ("8 12\n")
#we need to turn each of these numbers in an int
#we will start by converting this string into a list.
#str.split removes all whitespace by default, so we can use this to turn the string into a list of strings, each element being a seperate number
numList = line.split()

#the first # is the # of parts in the robot (0 through #-1)
#the second # is the # of steps needed to build the robot

numParts = int(numList[0])
numSteps = int(numList[1])

#now we have the number of parts and steps, what we want to do now, is skip over the actual instructions, and get to the single numbers at the end
#we then want to add all of those numbers (sprocket cost) to a map corresponding to the part (which are in order)
#after that, we want to come back to where we are now, and then read the instructions

#the way I am planning on implementing this, is to simply save the instructions in an array, and do stuff with them after we
#read the single numbers and reach the end of file

instrList = []                  #Used to store the instuctions
currentPart = 0                 #used to index the parts when we come across them
partMap = {}                    #the part map used to store the cost for each part

line = f.readline()

#while we haven't reached the end of file
while line != "":
    
    #if there is a space in the line, we know that it is an instruction, so we can save it to the array of instructions
    if " " in line:
        line = line.split()
        instrList.append((int(line[0]), int(line[1])))         #Each instruction will be tuple of ints (#, #)
    
    #Else we have a part cost, so add it to the map and then iterate the current part 
    else:
        partMap[currentPart] = int(line)
        currentPart += 1
    
    line = f.readline()

print(partMap)
print(instrList)

#Now we have a map containing all of the part costs, and an array of instructions
#we will iterate through the instructions and do the following:
#the first number in the instruction is the part used in the second number in the instruction
#so we will do instr[1] = instr[1] + instr[0]
#we will get the values for the parts from the map, and update accordingly

for instr in instrList:
    partMap[instr[1]] += partMap[instr[0]]

#After completing all instructions, we can return the cost of the final part, which can be found easily with numParts - 1
print(partMap[numParts - 1])