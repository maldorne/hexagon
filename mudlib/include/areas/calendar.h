

#define SAVE_FILENAME "/save/calendar.o"
#define POS_YEAR_NAME 0
#define POS_YEAR_ADJECTIVE 1
#define POS_YEAR_GENDER 2

// Where the name and the adjective were taken from in the tables. A year
// keeps the position, not the word, so it reads in whatever language the
// mud is compiled in. Years named before this was stored keep their words.
#define POS_YEAR_NAME_INDEX 3
#define POS_YEAR_ADJECTIVE_INDEX 4

// A name is masculine, feminine, or feminine while taking the masculine
// article, as a spanish noun starting with a stressed 'a' does. A language
// without genders marks every name as masculine.
#define YEAR_GENDER_MASCULINE 1
#define YEAR_GENDER_FEMININE 2
#define YEAR_GENDER_FEMININE_MASCULINE_ARTICLE 3

// #define MILLISECONDS_PER_DAY 86400000
