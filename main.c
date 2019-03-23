#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

const char statistics_filename[] = "statistics.txt";
const char temp_statistics_filename[]="__temp__.txt";

enum MENU_OPTIONS {
    START_GAME=1,
    VIEW_STATISTICS=2,
    EXIT_OPTION=3,
    BACK_TO_PREV_MENU=4
};

enum GAME_MODE_OPTIONS {
    AGAINST_COMPUTER=1,
    TWO_PLAYERS=2
};

enum LEVELS {
    EASY=1,
    MODERATE=2,
    IMPOSSIBLE=3
};

enum GAME_STATES {
    DRAW=0,
    PLAYER1=1,
    PLAYER2=2,
    COMPUTER=2
};

enum BOOLEAN_VALUES {
    true=1,
    false=0
};

struct player_statistics {
    char *name;
    unsigned int wins;
    unsigned int loses;
    unsigned int draws;
};

void init_player(struct player_statistics*, char);
char player_exists(char[]);
char read_input(char, char, char[]);
void save_player_statistics(struct player_statistics*);
void print_bar();
void print_main_menu();
void print_menu_levels();
void clear_screen();
void start_against_computer(char[9], struct player_statistics*);
void start_multiplayer(char[9], struct player_statistics*, struct player_statistics*);
void print_board(char[9]);
void load_player_statistics(struct player_statistics*, char[]);
char computerMove(char[9], char);
char hasTictactoe(char[9]);
void board_update(char[9],char,char);
char takeEasyMove(char[9]);
char takeModerateMove(char[9]);
char takeImpossibleMove(char[9]);
char keep_playing();
void clear_buffer(void);
void print_player_statistics(struct player_statistics*);
void init_statistics_file(void);
void start_game(void);
void player_not_found_message(char[]);
char isComputer(char);
char isPlayer(char);
char minimax(char[9],char);
char game_is_over(char[9]);
void error_occured(char[]);
void print_game_controls();
char AttemptRow(char, char[9]);
char blockRow(char, char[9]);
char AttemptColumn(char, char[9]);
char blockColumn(char, char[9]);
char blockFirstDiag(char[9]);
char blockSecondDiag(char[9]);
char AttemptFirstDiag(char[9]);
char AttemptSecondDiag(char[9]);


void init_statistics_file(void) {
    fclose(fopen(statistics_filename,"a")); // just in case the file not exist.
}


// Επιστρεφει 1, αν το ονομα περιεχει μονο λατινικους χαρακτηρες (κεφαλαιους η πεζους) η αριθμους.
// Ενω 0, αν δεν περιεχει μονο αυτους τους χαρακτηρες.
int parse(char name[]) {
    if(!strlen(name))
        return false;
    while(*name!='\0') {
        if(!isalnum(*name))
            return false;
        name++;
    }
    return true;
}

void init_player(struct player_statistics *player, char nplayer) {
    char name[24];

    printf("Please enter a name for player %d\n", nplayer);
    printf("You can only use latin characters lower and upper a-z, A-Z and the numeric digits 0-9.\n");
    print_bar();

    scanf("%s", name);
    clear_buffer();
    while(!parse(name)) {
        printf("You can only use latin characters lower and upper a-z, A-Z and the numeric digits 0-9.\n");
        print_bar();
        scanf("%s", name);
        clear_buffer();
    }

    player->name = (char*)malloc((strlen(name)+1)*sizeof(char));

    if(player_exists(name)) {
        load_player_statistics(player,name);
    } else {
        strcpy(player->name,name);
        player->wins=0;
        player->loses=0;
        player->draws=0;
        save_player_statistics(player);
    }
}

char player_exists(char player_name[]) {
    FILE *pStatisticsFile;
    struct player_statistics tempPlayer;

    tempPlayer.name = (char*)malloc(sizeof(char)*24);
    if(tempPlayer.name==NULL)
        error_occured("* Error [player_exists:tempPlayer.name]: something happened during malloc.\n");

    pStatisticsFile=fopen(statistics_filename,"r");

    if(pStatisticsFile==NULL)
        error_occured("* Error [player_exists:fopen]: something happened during processing statistics record file.\n");

    while( fscanf(pStatisticsFile, "%u|%u|%u|%s", &tempPlayer.draws, &tempPlayer.wins, &tempPlayer.loses, tempPlayer.name)!=EOF ) {
        if(strcmp(tempPlayer.name, player_name)==0) {
            fclose(pStatisticsFile);
            free(tempPlayer.name);
            return true;
        }
    }

    free(tempPlayer.name);
    fclose(pStatisticsFile);
    return false;
}

void load_player_statistics(struct player_statistics *player, char name[]) {
    FILE *pStatisticsFile;
    struct player_statistics tempPlayer;

    tempPlayer.name = (char*)malloc(sizeof(char)*24);
    if(tempPlayer.name==NULL)
        error_occured("* Error [load_player_statistics:tempPlayer.name]: something happened during malloc.\n");

    pStatisticsFile=fopen(statistics_filename,"r");

    if(pStatisticsFile==NULL)
        error_occured("* Error [load_player_statistics:fopen]: something happened during processing statistics record file.\n");

    while( fscanf(pStatisticsFile, "%u|%u|%u|%s", &tempPlayer.draws, &tempPlayer.wins, &tempPlayer.loses, tempPlayer.name)!=EOF ) {
        if(strcmp(tempPlayer.name, name)==0) {
            strcpy(player->name, tempPlayer.name);
            player->draws=tempPlayer.draws;
            player->wins=tempPlayer.wins;
            player->loses=tempPlayer.loses;
            fclose(pStatisticsFile);
            free(tempPlayer.name);
            return;
        }
    }

    free(tempPlayer.name);
    fclose(pStatisticsFile);
}

void save_player_statistics(struct player_statistics *player) {
    FILE *tempFile, *n_pStatisticsFile;
    struct player_statistics tempPlayer;

    tempPlayer.name = (char*)malloc(sizeof(char)*24);
    if(tempPlayer.name==NULL)
        error_occured("* Error [save_player_statistics:tempPlayer.name]: something happened during malloc.\n");

    tempFile=fopen(statistics_filename, "r");
    n_pStatisticsFile=fopen(temp_statistics_filename, "w");

    if(tempFile==NULL || n_pStatisticsFile==NULL)
        error_occured("* Error [save_player_statistics:fopen]: something happened during processing statistics record file.\n");

    while( fscanf(tempFile, "%u|%u|%u|%s", &tempPlayer.draws, &tempPlayer.wins, &tempPlayer.loses, tempPlayer.name)!=EOF ) {
        if( strcmp(tempPlayer.name, player->name) ) {
            fprintf(n_pStatisticsFile, "%u|%u|%u|%s\n", tempPlayer.draws,
                                                        tempPlayer.wins,
                                                        tempPlayer.loses,
                                                        tempPlayer.name);
        }
    }
    fprintf(n_pStatisticsFile, "%u|%u|%u|%s\n",  player->draws,
                                                 player->wins,
                                                 player->loses,
                                                 player->name);
    free(tempPlayer.name);
    fclose(tempFile);
    fclose(n_pStatisticsFile);
    remove(statistics_filename);
    rename(temp_statistics_filename, statistics_filename);
}

char game_is_over(char board[9]) {
    int i;
    if(hasTictactoe(board))
        return true;
    for(i=0; i<9; i++)
        if(board[i]==0)
            return false;
    return true;
}

void print_bar() {
    printf(">> ");
}

void print_main_menu() {
    printf("\t\tWelcome to tic tac toe!\n\n\n");
    printf("\t\t\tMain menu\n");
    printf("\t1. Play a game\n");
    printf("\t2. View player statistics\n");
    printf("\t3. Quit\n");
    print_bar();
}

void print_game_controls() {
    printf("Enter the number of the cell you want to pick from below.\n");
    printf("\r1|2|3\n\
            \r4|5|6\n\
            \r7|8|9\n");
    print_bar();
}

char getValidMove(char board[9]) {
    char move;
    move=read_input('1','9',"* Please enter a valid move\n");
    while(board[move-1]!=0) {
        printf("* %dth cell is already picked!\n",move);
        move=read_input('1','9',"* Please enter a valid move\n");
    }
    return move;
}

void start_multiplayer(char board[9], struct player_statistics *player1, struct player_statistics *player2) {
    char *player_name;
    char translate_table[3]={' ','X','O'};
    char move, tictactoe,
         player_turn = PLAYER1;

    player_name=player1->name;

    while(!game_is_over(board)) {
        clear_screen();
        print_board(board);
        printf("%s is playing now.", player_name);
        print_game_controls();
        move=getValidMove(board);
        board_update(board, move, translate_table[player_turn]);
        switch(player_turn) {
            case PLAYER1: {
                player_name=player2->name;
                player_turn=COMPUTER;
                break;
            }
            case PLAYER2: {
                player_name=player1->name;
                player_turn=PLAYER1;
                break;
            }
        }
    }

    clear_screen();
    print_board(board);

    tictactoe=hasTictactoe(board);
    switch(tictactoe) {
        case DRAW: {
            player1->draws++;
            player2->draws++;
            printf("The game is draw\n");
            break;
        }
        case PLAYER1: {
            player1->wins++;
            player2->loses++;
            printf("%s won!\n", player1->name);
            break;
        }
        case PLAYER2: {
            player1->loses++;
            player2->wins++;
            printf("%s won!\n", player2->name);
            break;
        }
    }
    printf("%s you have in total %d draws, %d wins and %d loses\n",player1->name,
                                                                   player1->draws,
                                                                   player1->wins,
                                                                   player1->loses);
    printf("%s you have in total %d draws, %d wins and %d loses\n",player2->name,
                                                                   player2->draws,
                                                                   player2->wins,
                                                                   player2->loses);
}

void start_against_computer(char board[9] , struct player_statistics *player) {
    char option, move, tictactoe,
         player_turn = PLAYER1;

    print_menu_levels();
    option=read_input('1','3',"* Please enter a valid option from the menu.\n");

    while(!game_is_over(board)) {
        clear_screen();
        print_board(board);
        print_game_controls();

        switch(player_turn) {
            case PLAYER1: {
                move=getValidMove(board);
                board_update(board, move, 'X');
                player_turn=COMPUTER;
                break;
            }
            case COMPUTER: {
                move=computerMove(board,option);
                board_update(board, move, 'O');
                player_turn=PLAYER1;
                break;
            }
        }
    }

    clear_screen();
    print_board(board);

    tictactoe=hasTictactoe(board);
    switch(tictactoe) {
        case DRAW: {
            player->draws++;
            printf("The game is draw\n");
            break;
        }
        case PLAYER1: {
            player->wins++;
            printf("You won!\n");
            break;
        }
        case COMPUTER: {
            player->loses++;
            printf("Computer eliminated you!\n");
            break;
        }
    }
    printf("%s you have in total %d draws, %d wins and %d loses\n",player->name,
                                                                   player->draws,
                                                                   player->wins,
                                                                   player->loses);
}

/*
    Αν το ταμπλο ειναι το παρακατω:
    X|X|X
    O|O|-
    -|-|-
    * οπου υπαρχει παυλα(-) σημενει οτι ειναι κενο το εκαστοτε κελι.
    Τοτε η hasTictactoe θα επιστρεψει την κωδικη ονομασια του Χ, δηλαδη το 1.

    Γενικα αν υπαρχει τριλιζα με το γραμμα Χ επιστρεφει 1.
           αν υπαρχει τριλιζα με το γραμμα Ο επιστρεφει 2.
           αν δεν υπαρχει τριλιζα επιστρεφει 0.

*/
char hasTictactoe(char board[9]) {
    // check rows
    if(board[0]==board[1] && board[0]==board[2] && board[0]!=0)
        return board[0];
    if(board[3]==board[4] && board[3]==board[5] && board[3]!=0)
        return board[3];
    if(board[6]==board[7] && board[6]==board[8] && board[6]!=0)
        return board[6];
    //check columns
    if(board[0]==board[3] && board[0]==board[6] && board[0]!=0)
        return board[0];
    if(board[1]==board[4] && board[1]==board[7] && board[1]!=0)
        return board[1];
    if(board[2]==board[5] && board[2]==board[8] && board[2]!=0)
        return board[2];
    //check diagonals.
    if(board[0]==board[4] && board[0]==board[8] && board[0]!=0)
        return board[0];
    if(board[2]==board[4] && board[2]==board[6] && board[2]!=0)
        return board[2];
    return 0;
}

char minimax(char board[9], char player) {
    int i, totalScore,
                score;
    int p;

    if(isComputer(player))
        totalScore=-2;
    if(isPlayer(player))
        totalScore=2;

    if(game_is_over(board)) {
        p=hasTictactoe(board);
        if(isPlayer(p))   return -1;
        if(isComputer(p)) return +1;
                          return +0;
    }

    for(i=0; i<9; i++) {
        if(board[i]==0) {
            if(isComputer(player)) {
                board[i]=player;
                score=minimax(board, PLAYER1);
                if(score > totalScore) { // computer tries to maximize the score.
                    totalScore=score;
                }
                board[i]=0;
            }
            if(isPlayer(player)) {
                board[i]=player;
                score=minimax(board, COMPUTER);
                if(score < totalScore){ // player tries to minimize the score.
                    totalScore=score;
                }
                board[i]=0;
            }
        }
    }
    return totalScore;
}


/*
    Η συναρτηση isComputer επιστρεφει true, αν εχει σειρα ο υπολογιστης (Κωδικη ονομασια: COMPUTER η αλλιως 2).
    Αλλιως επιστρεφει false.
*/
char isComputer(char player) {
    return player==COMPUTER;
}

/*
    Η συναρτηση isPlayer επιστρεφει true, αν εχει σειρα ο παικτης (Κωδικη ονομασια: PLAYER1 η αλλιως 1).
    Αλλιως επιστρεφει false.
*/
char isPlayer(char player) {
    return player==PLAYER1;
}

/*
    Η συναρτηση board_update ενημερωνει το ταμπλο.
    Καθε θεση του ταμπλου αποθηκευει εναν ακεραιο απο το 0 εως το 2.
    Αν σε μια θεση του ταμπλου υπαρχει 0 σημενει οτι δεν υπαρχει γραμμα, δηλαδη ειναι κενη.
    Αν υπαρχει 1, αυτο σημενει οτι υπαρχει το γραμμα Χ.
    Αν υπαρχει 2, αυτο σημενει οτι υπαρχει το γραμμα Ο.
*/
void board_update(char board[9], char move, char letter) {
    if(letter==' ')
        board[move-1]=0;
    if(letter=='X')
        board[move-1]=1;
    if(letter=='O')
        board[move-1]=2;
}

char computerMove(char board[9], char option) {
    char move;
    switch(option) {
        case EASY: {
            move=takeEasyMove(board);
            break;
        }
        case MODERATE: {
            move=takeModerateMove(board);
            break;
        }
        case IMPOSSIBLE: {
            move=takeImpossibleMove(board);
            break;
        }
    }
    return move;
}

char takeEasyMove(char board[9]) {
    int availMoves[9],
        n=0;
    char move;
    for(int i=0; i<9; i++) {
        if(board[i]==0) {
            availMoves[n]=i+1;
            n++;
        }
    }
    if(n==0)
        return -1; // Επεστεψε -1, στην περιπτωση που δεν υπαρχει αλλη διαθεσιμη κινηση.
    move=availMoves[rand()%n];
    return move;
}

// Αν παει να κανει τριλιζα ο παικτης κανε block. Αλλιως παιξε τυχαια.
char takeModerateMove(char board[9]) {
    int i;

    for(i=1; i<=3; i++)
        if(AttemptRow(i,board))
            return blockRow(i, board)+1;
    for(i=1; i<=3; i++)
        if(AttemptColumn(i, board))
            return blockColumn(i,board)+1;

    if(AttemptFirstDiag(board))
        return blockFirstDiag(board)+1;
    if(AttemptSecondDiag(board))
        return blockSecondDiag(board)+1;

    return takeEasyMove(board);
}

char blockFirstDiag(char board[9]) {
    if(board[0]==0)
        return 0;
    if(board[4]==0)
        return 4;
    if(board[8]==0)
        return 8;

    return -1; // Σε περιπτωση λαθους.
}


char AttemptFirstDiag(char board[9]) {
    int sum_x=0,
        sum_o=0;

    sum_x = (board[0]==1)
          + (board[4]==1)
          + (board[8]==1);
    sum_o = (board[0]==2)
          + (board[4]==2)
          + (board[8]==2);

    if(sum_x==2 && sum_o==0)
        return true;
    return false;
}

char blockSecondDiag(char board[9]) {
    if(board[2]==0)
        return 2;
    if(board[4]==0)
        return 4;
    if(board[6]==0)
        return 6;

    return -1; // Σε περιπτωση λαθους.
}

char AttemptSecondDiag(char board[9]) {
    int sum_x=0,
        sum_o=0;

    sum_x = (board[2]==1)
          + (board[4]==1)
          + (board[6]==1);
    sum_o = (board[2]==2)
          + (board[4]==2)
          + (board[6]==2);

    if(sum_x==2 && sum_o==0)
        return true;
    return false;
}

char blockRow(char row_number, char board[9]) {
    int i, beg;

    beg=3*row_number-3; // βλεπε σχολια της AttemptRow.
    for(i=0; i<3; i++)
        if(board[beg+i]==0)
            return beg+i;

    return -1; // Σε περιπτωση λαθους.
}

char AttemptRow(char row_number, char board[9]) {
    /*
        Το πρωτο στοιχειο της 1ης γραμμης του board ειναι στην θεση 0
        Το πρωτο στοιχειο της 2ης γραμμης του board ειναι στην θεση 3
        Το πρωτο στοιχειο της 3ης γραμμης του board ειναι στην θεση 6
        Ειναι φανερο πως η σχεση μεταξυ της θεσης του πρωτου στοιχειου της n-οστης γραμμης του board με τον αριθμο n
        ειναι η 3*(n-1)=3*n-3, πραγματι για n=1 => πρωτο στοιχειο στην θεση 3*n-3=3*1-3=0
                                        για n=2 => πρωτο στοιχειο στην θεση 3*n-3=3*2-3=6-3=3
                                        για n=3 => πρωτο στοιχειο στην θεση 3*n-3=3*3-3=9-3=6
    */
    int sum_x=0,
        sum_o=0;
    int beg;

    beg=3*row_number-3;
    sum_x=(board[beg+0]==1)
         +(board[beg+1]==1)
         +(board[beg+2]==1);
    sum_o=(board[beg+0]==2)
         +(board[beg+1]==2)
         +(board[beg+2]==2);

    if(sum_x==2 && sum_o==0)
        return true;
    return false;
}


char blockColumn(char column_number, char board[9]) {
    int i, beg;

    beg=column_number-1; // βλεπε σχολια της AttemptColumn.
    for(i=0; i<3; i++)
        if(board[beg+i*3]==0)
            return beg+i*3;

    return -1; // Σε περιπτωση λαθους.
}

char AttemptColumn(char column_number, char board[9]) {
    /*
        Το πρωτο στοιχειο της 1ης στυλης του board ειναι στην θεση 0
        Το πρωτο στοιχειο της 2ης στυλης του board ειναι στην θεση 1
        Το πρωτο στοιχειο της 3ης στυλης του board ειναι στην θεση 2
        Ειναι φανερο πως η σχεση μεταξυ της θεσης του πρωτου στοιχειου της n-οστης στυλης του board με τον αριθμο n
        ειναι η n-1, πραγματι για n=1 => πρωτο στοιχειο στην θεση n-1=1-1=0
                              για n=2 => πρωτο στοιχειο στην θεση n-1=2-1=1
                              για n=3 => πρωτο στοιχειο στην θεση n-1=3-1=2
    */

    int sum_x=0,
        sum_o=0;
    int beg;

    beg=column_number-1;
    sum_x=(board[beg+0]==1)
         +(board[beg+3]==1)
         +(board[beg+6]==1);
    sum_o=(board[beg+0]==2)
         +(board[beg+3]==2)
         +(board[beg+6]==2);

    if(sum_x==2 && sum_o==0)
        return true;
    return false;
}



char takeImpossibleMove(char board[9]) {
    char move, totalScore,
                    score;
    int i;
    totalScore=-2;
    for(i=0; i<9; i++) {
        if(board[i]==0) {
            board[i]=COMPUTER;
            score=minimax(board, PLAYER1);
            if(score > totalScore) {
                totalScore=score;
                move=i;
            }
            board[i]=0;
        }
    }
    return move+1;
}


void print_menu_levels() {
    clear_screen();
    printf("\t\tMenu levels\n");
    printf("\t\tIn which level do you want to play?\n");
    printf("\t\t1. Easy\n");
    printf("\t\t2. Moderate\n");
    printf("\t\t3. Impossible\n");
    print_bar();
}


void clear_buffer(void) {
    while ( getchar()!='\n' );
}

char read_input(char lowerBound, char upperBound, char msg[]) {
    char choice;
    choice=getchar();
    if(choice!='\n') clear_buffer(); // αν πατηθει μονο enter, ο buffer ειναι καθαρος και ετσι δεν χρειαζεται να τον καθαρισω.
    while(choice>upperBound || choice <lowerBound) {
        printf(msg);
        print_bar();
        choice=getchar();
        if(choice!='\n') clear_buffer();
    }
    return choice-'0';
}


void clear_screen() {
    system("cls");
}

void init_board(char board[9]) {
    for(int i=0; i<9; i++)
        board[i]=0;
}

void print_board(char board[9]) {
    char translate_table[3]={' ','X','O'};
    printf("\r%c|%c|%c\n\
            \r-----\n\
            \r%c|%c|%c\n\
            \r-----\n\
            \r%c|%c|%c\n",translate_table[board[0]], translate_table[board[1]], translate_table[board[2]],
                          translate_table[board[3]], translate_table[board[4]], translate_table[board[5]],
                          translate_table[board[6]], translate_table[board[7]], translate_table[board[8]]);
}

char keep_playing() {
    char ch;
    printf("Do you want back to main menu?\nPress y for yes and n for no");
    do {
        putchar('\n');
        print_bar();
        ch=getchar();
        if(ch!='\n') clear_buffer();
    } while(ch!='y'&&ch!='n');
    return ch=='n'; // εαν ειναι το ch n τοτε true αλλιως false
}

void error_occured(char error_msg[]) {
    perror(error_msg);
    exit(EXIT_FAILURE);
}

void exit_game() {
    printf("Thank you for playing tic tac toe!\n");
    exit(EXIT_SUCCESS);
}

void print_menu() {
    printf("\tWelcome to tic tac toe's modes menu.\n");
    printf("\n");
    printf("\tDo you want to play with whom?\n");
    printf("\t1. Against me?\n");
    printf("\t2. Or against your friend?\n");
    printf("\t3. Or do you want to leave?\n");
    printf("\t4. Or do you want to go to main menu?\n");
    print_bar();
}

void free_player_statistics(struct player_statistics* player) {
    free(player->name);
    free(player);
}

void start_game(void) {
    char board[9];
    char option, game_is_running;
    struct player_statistics *player1,
                             *player2;
    player1 = (struct player_statistics*)malloc(sizeof(struct player_statistics));
    if(player1==NULL)
        error_occured("* [start_game:player1]: something happened during malloc.\n");

    init_player(player1, PLAYER1);
    do {
        clear_screen();
        init_board(board);
        print_menu(); // print_game_menu
        option=read_input('1','4',"* Please enter a valid option from the menu\n");
        switch(option) {
            case AGAINST_COMPUTER: {
                start_against_computer(board, player1);
                save_player_statistics(player1);
                break;
            }
            case TWO_PLAYERS: {
                player2 = (struct player_statistics*)malloc(sizeof(struct player_statistics));
                if(player1==NULL)
                    error_occured("* [start_game:player1]: something happened during malloc.\n");
                init_player(player2,PLAYER2);
                while( strcmp(player2->name, player1->name) == 0 ){
                    printf("* You can't play with the same name with player 1.\n");
                    init_player(player2,PLAYER2);
                }
                start_multiplayer(board, player1, player2);
                save_player_statistics(player1);
                save_player_statistics(player2);
                free_player_statistics(player2);
                break;
            }
            case EXIT_OPTION: {
                free_player_statistics(player1);
                exit_game();
                break;
            }
            case BACK_TO_PREV_MENU: {
                free_player_statistics(player1);
                return;
                break;
            }
        }
        game_is_running=keep_playing();
    } while(game_is_running);
}

void print_player_statistics(struct player_statistics *player) {
    printf("Player '%s' has:\n", player->name);
    printf("%d draws.\n", player->draws);
    printf("%d wins.\n", player->wins);
    printf("%d loses.\n", player->loses);
}

void view_statistics() {
    struct player_statistics tPlayer;
    char name[24];

    clear_screen();
    printf("\tWelcome to tic tac toe statistic search.\n");
    printf("\n");
    printf("\tPlease enter a player name: \n");
    print_bar();

    scanf("%s",name);
    clear_buffer();
    while(!parse(name)) { // Ελενχος, αν το κειμενο περιεχει λατινικους μονο χαρακτηρες και αριθμητικα ψηφια.
        printf("A player name consists from latin characters (lower and upper) and numeric digits.\n");
        print_bar();
        scanf("%s",name);
        clear_buffer();
    }

    if(player_exists(name)) {
        tPlayer.name = (char*) malloc((strlen(name)+1)*sizeof(char));
        load_player_statistics(&tPlayer,name);
        print_player_statistics(&tPlayer);
        free(tPlayer.name);
    } else {
        player_not_found_message(name);
    }
    system("pause");
}

void player_not_found_message(char name[]) {
    printf("\tSorry, it seems player '%s' has never played tic tac toe before.\n",name);
    printf("\tTry with an another player name.\n");
}

int main()
{
    char option;
    srand(time(NULL));
    init_statistics_file();
    do {
        clear_screen();
        print_main_menu();
        option=read_input('1','3', "* Please enter a valid option from the menu.\n");

        switch(option) {
            case START_GAME: {
                start_game();
                break;
            }
            case VIEW_STATISTICS: {
                view_statistics();
                break;
            }
        }
    } while(option!=EXIT_OPTION);
    printf("Thank you for playing tic tac toe!\n");
    return EXIT_SUCCESS;
}
