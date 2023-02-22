CSCI 0300 Project 1 - Snake
===========================

## Design Overview:
To parse through a custom board dimension, I first interpreted the dimensions of the board
by adding one each time to the address reference so that it would reference the memory of the
bopard String minus the first value. I then used strtok to iterate through the delimeters.  
- To interpret numbers, I used atoi

I also made use of a doubly linked list to represent the logical
locations of all my snakes

## Collaborators: William Park, Andrew Chen

## Responsible Computing:
Partner CS login: azchen

1. The unicode consortium is a nonprofit organization dedicated with maintaining the software standards that are used universally for unicode. Members of this
consortium includes corporations, software producers, government ministires, research institutions, and involved individuals. They have various committees 
overseeing their standards and dissemination of standards, and are based in California. They are run primarily by American based entities, particularly
big tech corporations, which may lead to these companies producing unicode languages that are favorable to their software, but not neccesarily compatible or widely known by others outside the US and Western world.
2. One language that has only been recently added to Unicode are Kaktovik numerals, which are used by the Inuit and Yupik languages. It was added as of Unicode's 15.0 version. In addition, 4,193 CJK ideographs were added. THis is significant as it now allows indiviudals who use the languages which were modified
in the recent update to use a system that is more familiar to them, rather than have to adhere to another one. This adds for more efficiency, as less conversions are required. 
3. Partner name: Andrew Chen 
    A) Argument against Han Unification: I argue the Han unification would lead to a lack of cultural identity for these East Asian subcultures. To use an ancient character alphabet like the Han unification runs under the distinction that Korean, Japanese, and Chinese culture are closely linked enough to unify them under one language, yet in reality these are deeply distinct societies with developed differences in their alphabet. Korea and Japan have developed their own distinct cultures since they were unified under the Han dynasty, and the development of unique languages is an instance of this. To go back to an ancient language will erase all of this, and force native languages to adopt a new language that doesn’t reflect their culture in certain instances, as illustrated by the difference in the Han unification word for grass vs the Korean and Japanese. All in all, these countries have developed independent of each other for centuries, and to revert back to an old language does not reflect the diversity of various modern cultures
    2. Andrew’s argument: Han unification is an imperfect but necessary abstraction in digital communication
    * My argument: in exchange for efficiency and uniformity across multiple devices, there is the possibility that this will create entry/language barriers for Koreans and Japanese people entering the systems field. It may also be less intuitive for them to implement Unicode, and can lead to small syntactical errors which could be significant
    * It goes against modern principles of embracing diversity, and molding distinct and cultures that have developed indenepdently for centuries back into one ancient system goes against the independence of these distinct countries.
    * Counterclaim: To a degree, yes, but I don’t think in a substantial way that hinders general communication as the languages do bear significant resemblance
    * Ultimately, it is true that the perceived benefits from the homogeneity of the system actually snuffs out the cultural distinctions that make the region so diverse. However, the question begs if there are alternative representations that would reflect the modernity that you emphasized.

## Extra Credit attempted:
Custom board:
Nelson Center Lecture Hall: "B10x20|W20|W1E18W1|W1E18W1|W8E2W1E3W6|W1E4W1E4W1E8W1|W1E1W2E1W1E4W1E8W1|W1E4W8E2W1E3W1|W1E1S1E12W1E3W1|W1E14W1E3W1|W20" 

## How long did it take to complete Snake?: 30

<!-- Enter an approximate number of hours that you spent actively working on the project. -->
