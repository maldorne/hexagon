
// Adventuring parties: a handful of livings who band together for a while,
// with a channel of their own. A party lives only as long as the mud runs.

#define PARTY_HANDLER "/lib/handlers/parties"

// how many can stand in one party, the leader included
#define MAX_PARTY_MEMBERS 5

// how long a party name may be
#define PARTY_NAME_LENGTH 60

// how long a character has to have been played before it can start a party
#define PARTY_MIN_PLAY_TIME (5 * 3600)

// an invitation, and how long it stands. A timed property counts heart beats,
// not seconds, so thirty of them is about a minute.
#define PARTY_INVITE_PROP "party_invitation"
#define PARTY_INVITE_TIME 30
