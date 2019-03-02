#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h> // pow()
#include <assert.h>
#define NUMBER_OF_COLORS 6

//
//  ProjetC
//
//  Created by Pierre Schutz on 10.04.18.
//  Copyright © 2018 Pierre Schutz. All rights reserved.
//

// ==== Color ==========================================================
/**
 @brief enumeration of all colors in MasterMind
 */
typedef enum {YELLOW, BLUE, GREEN, RED, PINK, ORANGE} color;

/** --------------------------------------------------
 * @brief find the next color in the order defined.
 * @param c the current color.
 * @return the next color
 */
color nextColor(color c)
{
    return (NUMBER_OF_COLORS - 1 == c) ? YELLOW : c + 1;
}


// ==== BitSet ==========================================================



/**
 @brief this struct define an array of bits.
 */
typedef struct {
    size_t size ;
    char* bytes ;
} BitSet;


/** --------------------------------------------------
 * @brief creates a BitSet structure of a given size, specified as the number of wanted bits.
 * @param size number of wanted bits
 * @return the newly created (dynamically allocated) BitSet structure
 */
BitSet create_bitset(size_t size)
{
    BitSet result = { 0, NULL };
    const size_t extra = size % 8 > 0 ? 1 : 0;
    result.bytes = calloc(size / 8 + extra, sizeof(char));
    if (result.bytes != NULL) {
        result.size = size;
    }
    return result;
}

/** --------------------------------------------------
 * @brief gets the bit value at bit position `index`
 * @param bitset the BitSet structure to read from
 * @param index the bit position to read
 * @return 0 or 1 according to bit value
 */
int bitset_get(const BitSet bitset, size_t index)
{
    if (index >= bitset.size) return 0;
    return (bitset.bytes[index / 8] >> (index % 8)) & 0x1;
}

/** --------------------------------------------------
 * @brief count the number of bits at state 1 un the array.
 * @param bitset the BitSet structure to read from.
 * @return count of bits at 1.
 */
int bitset_count1(const BitSet bitset)
{
    int count = 0;
    for (int i = 0; i < bitset.size; ++i) {
        count += bitset_get(bitset, i);
    }
    return count;
}

/** --------------------------------------------------
 * @brief destroy in memory given bitset
 * @param bitset the BitSet .
 */
void destroy_bit_set(BitSet b) {
    free( b.bytes);
}

/** --------------------------------------------------
 * @brief sets the bit value at bit position `index`
 * @param bitset the BitSet structure to write to
 * @param index the bit position to overwrite
 * @param bit 0 or 1 value to be set
 */
void bitset_set(BitSet* bitset, size_t index, int bit)
{
    assert(bitset != NULL); // this is a tool function
    if (index > bitset->size) return;
    char* prev = bitset->bytes + (index / 8);
    char shifted = 0x1 << (index % 8);
    *prev = bit ? (*prev | shifted) : (*prev & (-1 ^ shifted));
}


// ==== Combination =====================================================


/** --------------------------------------------------
 * @brief define a Combination of MasterMind.
 */
typedef struct  {
    size_t size ;
    color* elements ;
} Combination ;

/** --------------------------------------------------
 * @brief creates a new combination whith only Yellow
 * @param size the size of the combination
 * @return the creatd combination
 */
Combination create_combination(size_t size)
{
    
    color* newelements = calloc(size, sizeof(color));
    for (int i = 0; i < size; ++i) {
        newelements[i] = YELLOW;
    }
    Combination newCombination = {size, newelements};
    return newCombination;
}

/** --------------------------------------------------
 * @brief creates a BitSet structure of a given size, specified as the number of wanted bits.
 * @param c the combination to destroy
 */
void destroy_combination(Combination c) {
    free(c.elements);
}

/** --------------------------------------------------
 * @brief find the next combination of current.
 * @param current the current combination
 * @return the next combination in the order of colors.
 */
int nextCombination(Combination* current)
{
    size_t i = 0;
    while (i < current->size) {
        switch (current->elements[i]) {
            case ORANGE:
                if (i == current->size - 1) {
                    return 0;
                }
                current->elements[i] = nextColor(current->elements[i]);
                ++i;
                break;
                
            default:
                current->elements[i] = nextColor(current->elements[i]);
                i = current->size;
                
                break;
        }
    }
    return 1;
}



/** --------------------------------------------------
 * @brief transform the combination to an integer that represents his index.
 * @param c the combination to transform.
 * @return the index of the combination
 */
size_t combination_to_index(const Combination c)
{
    size_t index = 0;
    for (int i = 0; i < c.size ; ++i) {
        index += c.elements[i]*pow(NUMBER_OF_COLORS, i);
    }
    return index;
    
}

/** --------------------------------------------------
 * @brief transform the index to the corresponding combination
 * @param index the index of the combination
 * @param combination the object used to upadate the combination
 * @return the combination corresponding to the index in input
 */
Combination* combination_from_index(size_t index, Combination* combination)
{
    size_t updatedIndex = index;
    for (int i = 0; i < combination->size; ++i) {
        combination->elements[i] = updatedIndex % NUMBER_OF_COLORS;
        updatedIndex = updatedIndex / NUMBER_OF_COLORS;
    }
    return combination;
}

/**
* @brief print a string corresponding to the combination
* @param combination the object to print
 */
void print_combination(const Combination combination)
{
    for (size_t i = 0; i < combination.size; ++i) {
        if (i > 0) putchar(' ');
        switch(combination.elements[i]) {
            case YELLOW:
                putchar('Y');
                break;
            case BLUE:
                putchar('B');
                break;
            case GREEN:
                putchar('G');
                break;
            case RED:
                putchar('R');
                break;
            case PINK:
                putchar('P');
                break;
            case ORANGE:
                putchar('O');
                break;
            default:
                printf("\nERROR\n");
                printf("Unexpected color %d\n", combination.elements[i]);
        }
    }
    putchar('\n');
}


// ==== Answer ==========================================================

/** --------------------------------------------------
 * @brief the struct represents an Anwser in MasterMind
 */
typedef struct {
    int positions;
    int colors;
} Answer;

/** --------------------------------------------------
 * @brief ask to the user to test the given combination
 * @param c the combination to test
 * @param a the object used to return answer
 * @return 0 if we need to comtinue the search, 1 otherwire (combination found or error in user response)
 */
int ask(Combination c, Answer* a) {
    const int CORRECT_SCAN = 2;
    print_combination(c);
    printf("Please score attempt (positions, colors): ");
    int test;
    fflush(stdout);
    
    test = scanf("%d %d", &a->positions, &a->colors);
    
    if (CORRECT_SCAN != test) {
        printf("Unable to parse answer. Aborting.");
        return 1;
    } else if (a->positions == c.size) {
        printf("Found solution : ");
        print_combination(c);
        return 1;
    }
    
    return 0;
    
}



// ==== Solvers =========================================================

/** --------------------------------------------------
 * @brief This struct represntend a Solver_support to find the right combination.
 */
typedef struct {
    Combination currentTry;
    Combination util;
    BitSet b;
    
} Solver_support;

/** --------------------------------------------------
 * @brief create a new Solver support using current combination.
 * @param current the actual combination
 * @return the created solver support.
 */
Solver_support create_solver_support(Combination current) {
    BitSet b = create_bitset(pow(NUMBER_OF_COLORS, current.size));
    for (int i = 0; i < b.size; ++i) {
        bitset_set(&b, i, 1);
    }
    Solver_support s = {current, create_combination(current.size), b };
    
    return s;
}

/** --------------------------------------------------
 * @brief destroy solver support in memory
 * @param s the solver support to destroy
 */
void destroy_solver_support(Solver_support s) {
    destroy_bit_set(s.b);
    destroy_combination(s.currentTry);
    destroy_combination(s.util);
}

/** --------------------------------------------------
 * @brief find the next available combination after the one giver as parameter
 * @param s the solver_support used to find the combination
 * @return the next valid combination after the current one.
 */
int nextGoodCombination(Solver_support s) {
    while (nextCombination(&s.currentTry) == 1) {
        if (bitset_get(s.b, combination_to_index(s.currentTry)) == 1) {
            return 1;
        }
    }
    return 0;
}


/** --------------------------------------------------
 * @brief Brute Forcer solver of the game
 * @param size the size of user's combination
 */
void solve_brute_force(size_t size)
{
    Combination c = create_combination(size);
    Answer a = {0, 0} ;
    while (ask(c, &a) == 0 && nextCombination(&c) != 0){}
    destroy_combination(c);
}

/** --------------------------------------------------
 * @brief transform the index to the corresponding combination
 * @param attempt the combination to test
 * @param result the combination to compare with (considered as solution)
 * @param ans the object used to store the difference between test and solution, so the Answer.
 * @return 0 if the is any error and 1 otherwise
 */
int score_attempt(const Combination* attempt, const Combination* result, Answer* ans)
{
    if (ans     == NULL) return 0;
    ans->colors    = 0;
    ans->positions = 0;
    if (result  == NULL) return 0;
    if (attempt == NULL) return 0;
    if (attempt->size != result->size) return 0;
    
    int* used = calloc(attempt->size, sizeof(int));
    if (used == NULL) return 0;
    
    // Scoring positions
    for (size_t i = 0; i < attempt->size; ++i) {
        if (attempt->elements[i] == result->elements[i]) {
            ++(ans->positions);
            used[i] = 1;
        }
    }
    
    // Scoring colors
    for (size_t i = 0; i < attempt->size; ++i) {
        if (attempt->elements[i] != result->elements[i]) {
            int found = 0;
            for (size_t j = 0; !found && (j < attempt->size); ++j) {
                if (i != j && !used[j] && (attempt->elements[i] == result->elements[j])) {
                    ++(ans->colors);
                    used[j] = 1;
                    found = 1;
                }
            }
        }
    }
    
    free(used);
    return 1;
}


/** --------------------------------------------------
 * @brief find which combination are already valid or not
 * @param s the solver_support used to find result
 * @param count the number of remaining valid combination
 * @return 0 if there is an error, 1 otherwise
 */
int review_combinations(Solver_support* s, size_t* count)
{
    assert(s != NULL); // review_combination() is a tool function so this should never happen
    Answer userAnswer = {0, 0};
    
    if (1 == ask(s->currentTry, &userAnswer)){
        return 0;
    } else {
        for (int i = 0; i < pow(NUMBER_OF_COLORS, s->currentTry.size); ++i) {
            
            Answer test = {0, 0};
            Combination util = create_combination(s->currentTry.size);
            score_attempt(&s->currentTry, combination_from_index(i, &util), &test);
            if (test.colors != userAnswer.colors || test.positions != userAnswer.positions) {
                bitset_set(&s->b, i, 0);
                
                if (count != NULL) {
                    int c = (int) *count;
                    c -= 1;
                    *count = (size_t) c;
                    
                }
            }
        }
        
    }
    return 1;
}


static int FOUND_ANSWER_OR_ERROR = 0;

/** --------------------------------------------------
 * @brief Solve the game used BitSet and try only valid combination
 * @param size the size of user's combination
 */
void solve_with_bitset(size_t size)
{
    Combination c = create_combination(size);
    Solver_support s = create_solver_support(c);
    while (review_combinations(&s, NULL) != FOUND_ANSWER_OR_ERROR){
        nextGoodCombination(s);
    }
    destroy_combination(c);
    destroy_solver_support(s);
}


/** --------------------------------------------------
* @brief find the max of two integer
* @param i1 the first integer to compare
* @param i2 the second integer to compare
* @return the maximum of them
*/
int find_max(int i1, int i2) {
    return (i1 > i2) ? i1 : i2;
}

/** --------------------------------------------------
 * @brief upgrate the maximum value and combination
 * @param bestC the best combination
 * @param currentC the current combination
 * @param s the solver_support
 * @param count the number of remaining valid combinations
 * @param max the maximum cardinal of remaing combination for some score
 * @param maxValue the maximum of the mininum set of remaing combination over all combination
 * @param firstCombination and integer that tests if there is the first tested combination
 */
void upgradeMax(Combination* bestC,Combination* currentC,
                Solver_support* s, size_t count, int max, int* maxValue, int firstCombination)
{
    //Upgrading maxValue and bestCombination
    size_t indexBest = combination_to_index(*bestC);
    size_t indexCurr = combination_to_index(*currentC);
    
    int bitSetBest = bitset_get(s->b, indexBest);
    int bitSetCurr = bitset_get(s->b, indexCurr);
    
    int newValue = (int)count - max;
    if (*maxValue < newValue) {
        *bestC = *currentC;
    } else if (*maxValue == newValue) {
        // Choosing best combination in function of validity and index.
        switch (bitSetBest) {
            case 1:
                switch (bitSetCurr) {
                    case 1:
                        if (indexBest > indexCurr) {
                            *bestC = *currentC;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case 0 :
                switch (bitSetCurr) {
                    case 1:
                        *bestC = *currentC;
                        break;
                    case 0:
                        if (indexBest > indexCurr) {
                            *bestC = *currentC;
                        }
                    default:
                        break;
                }
                break;
                
            default:
                break;
        }
        
    }
    
    *maxValue = find_max(newValue, *maxValue);
    
}

/** --------------------------------------------------
 * @brief choose the best next combination over all combinations
 * @param s the solver support that help us to find the answer
 * @param count the number of valid remaining combination
 * @return 0 if there is an error in parameter and 1 otherwise
 */
int chooseBestCombination(Solver_support* s, size_t* count)
{
    if ( s == NULL) return 0;
    if (count == NULL) return 0;
    const size_t combinationSize = s->currentTry.size;
    const int NUMBER_OF_ANSWERS = pow(combinationSize + 1, 2);
    const int NUMBER_OF_COMBINATIONS = pow(NUMBER_OF_COLORS, s->currentTry.size);
    int maxValue = 0;
    Combination bestCombination = create_combination(s->currentTry.size);
    int firstCombination = 1;
    
    //Interating over all possible combinations
    for (int i = 0; i < NUMBER_OF_COMBINATIONS; ++i) {
        Combination current = create_combination(combinationSize);
        combination_from_index(i, &current);
        
        
        // Dynamic table definition and initialisation
        int* dynamicTable = calloc(NUMBER_OF_ANSWERS, sizeof(int));
        for (int j = 0; j < NUMBER_OF_ANSWERS; ++j) {
            dynamicTable[j] = 0;
        }
        
        //Calculation of cardinality of Hi(s)
        Answer a = {0, 0};
        int max = 0;
        
        
        for (int j = 0; j < NUMBER_OF_COMBINATIONS; ++j) {
            
            
            if (bitset_get(s->b, j) == 1) {
                Combination test = create_combination(combinationSize);
                combination_from_index(j, &test);
                score_attempt(&test, &current, &a);
                dynamicTable[a.colors + (combinationSize + 1)*a.positions] += 1;
            }
        }
        
        
        //Computation of the biggest cardinality of Hi(s)
        for (int j = 0; j < pow(combinationSize + 1, 2); ++j) {
            max = find_max(max, dynamicTable[j]);
        }
        
        //Killing our table in memory
        free(dynamicTable);
        //Upgrading maxValue and bestCombination
        if (firstCombination == 1){
            firstCombination = 0;
            bestCombination = current;
            maxValue = (int)count - max;
        } else {
            upgradeMax(&bestCombination, &current, s, *count, max, &maxValue, firstCombination);
        }
        
    }
    s->currentTry = bestCombination;
    return 0;
}


/** --------------------------------------------------
 * @brief Solve the MasterMind game using Knuth algorithm
 * @param size the size of the combination
 */
void solve_knuth(size_t size)
{
    //Initialisation of arguments
    Combination currTry = create_combination(size);
    Solver_support s = create_solver_support(currTry);
    
    size_t index = 0;
    size_t available = bitset_count1(s.b);
    
    //Initialisation of the currentCombination to YB, YYB, YYBB or YYYBB.
    for(int i = 0; i < size / 2; ++i) {
        index += pow(NUMBER_OF_COLORS, i+ (size+1)/2);
    }
    s.currentTry = *combination_from_index(index, &s.util);
    
    while (review_combinations(&s, &available) == 1){
        chooseBestCombination(&s, &available);
    }
    destroy_combination(currTry);
    destroy_solver_support(s);
    
    
    
}




// ==== main() ==========================================================

int main(void)
{
    size_t size = 0;
    
    do {
        printf("What size (2, 3, 4, 5)? ");
        fflush(stdout);
        scanf("%zu", &size);
    } while ((size != 2) && (size != 3) && (size != 4) && (size != 5));
    
    char strategy = '\0';
    do {
        printf("What strategy ([B]rute force, B[i]tfield, [K]nuth)? ");
        fflush(stdout);
        scanf(" %c", &strategy);
    } while ((strategy != 'B') && (strategy != 'i') && (strategy != 'K'));
    
    switch(strategy) {
        case 'B':
            solve_brute_force(size);
            break;
            
        case 'i':
            solve_with_bitset(size);
            break;
            
        case 'K':
            solve_knuth(size);
            break;
    }
    
    return 0;
}
