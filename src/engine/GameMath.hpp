#ifndef GAME_MATH_HPP
#define GAME_MATH_HPP

#include <cmath>

// mathematical constants
#define PI              3.141592654f
#define ROOT2           1.414213562f
#define SIN45           0.7071067812f
#define EULERS_NUMBER   2.7182818285f



namespace Math 
{




    // a two dimensional vector
    struct Vector2 {
        float x;
        float y;


        // constructors
        Vector2( float X, float Y );
        Vector2();



        // used for finding length and getting unit vectors
        float length();
        void normalise(); // makes itself a unit vector
        Vector2 normalised(); // returns itself as a unit vector



        // overload operators
        Vector2 operator+ ( Vector2 const& other );
        Vector2 operator- ( Vector2 const& other );
        Vector2 operator* ( float const& k );
        Vector2 operator/ ( float const& k );
        float   operator* ( Vector2 const& other );
        bool    operator==( Vector2 const& other );
        bool    operator!=( Vector2 const& other );
        void    operator+=( Vector2 const& other );
        void    operator-=( Vector2 const& other );
        void    operator/=( float const& k );
        void    operator*=( float const& k );


    };

    // global 2D vectors, Vector2_Zero = zero magnitude vector, Vector2_One = (1, 1)
    extern Vector2 Vector2_Zero;
    extern Vector2 Vector2_One;
    extern Vector2 Vector2_Up;
    extern Vector2 Vector2_Down;
    extern Vector2 Vector2_Left;
    extern Vector2 Vector2_Right;










    // a Vector2, but with integer entries
    struct Vector2Int {
        int x;
        int y;


        // contructors
        Vector2Int( int X, int Y );
        Vector2Int();


        // overload operators
        Vector2Int operator+ ( Vector2Int const& other );
        Vector2Int operator- ( Vector2Int const& other );
        Vector2Int operator* ( float const& k );
        Vector2Int operator/ ( float const& k );
        int        operator* ( Vector2Int const& other );
        bool       operator==( Vector2Int const& other );
        bool       operator!=( Vector2Int const& other );
        void       operator+=( Vector2Int const& other );
        void       operator-=( Vector2Int const& other );
        void       operator/=( float const& k );
        void       operator*=( float const& k );


    };

    // global Vector2Ints
    extern Vector2Int Vector2Int_Zero;
    extern Vector2Int Vector2Int_One;
    extern Vector2Int Vector2Int_Up;
    extern Vector2Int Vector2Int_Down;
    extern Vector2Int Vector2Int_Left;
    extern Vector2Int Vector2Int_Right;





    // finds the unit vector pointing from p0 to p1
    Vector2 getUnitVector( Vector2 p0, Vector2 p1 );

    // finds the hadamard product of two vectors
    Vector2 hadamard( Vector2 v0, Vector2 v1 );



    // math functions

    float minf( float a, float b ); // min for floats
    float maxf( float a, float b ); // max for floats

    int Max( int a, int b ); // max for ints
    int Min( int a, int b ); // min for ints


    float absf( float x ); // abs for floats
    int Abs( int x ); // abs for ints


    float clampf(float min, float max, float x); // clamps x between 2 values, for floats
    int Clamp(int min, int max, int x); // clamp for ints


    // rounds up/down a float to the nearest integer
    float ceil( float x );
    float floor( float x );
    // returns it as an int
    int ceilToInt( float x );
    int floorToInt ( float x );


    int signf( float a ); // finds the sign of a float
    int Sign( int a );    // sign for ints


    float powf( float x, int n ); // finds x to the nth power
    int Pow( int x, int n );      // pow for ints


    int fact( int x ); // finds x!




    // counts the number of active bits in a variable
    int numBits(int var);
}

#endif