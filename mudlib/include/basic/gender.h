
// Gender ids. These are the driver's own values: query_gender() and set_gender()
// speak them, and they are what a saved object stores, so the numbers are part
// of the data format rather than an implementation detail -- per-gender data is
// keyed by the id elsewhere (a bestiary template indexes its gendered fields by
// "" + gender). Neuter belongs to objects: gender.c refuses to set it on a
// living.

#define GENDER_NEUTER   0
#define GENDER_MALE     1
#define GENDER_FEMALE   2
