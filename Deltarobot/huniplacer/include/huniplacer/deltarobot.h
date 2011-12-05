#pragma once

#include <huniplacer/point3.h>
#include <huniplacer/imotor3.h>
#include <huniplacer/effector_boundaries.h>

//TODO: implement forward kinematics and use that to calculate the current effector position

/**
 * @brief holds huniplacer related classes
 **/
namespace huniplacer
{
	class inverse_kinematics_model;

	/**
	 * @brief this class symbolises an entire deltarobot
	 **/
    class deltarobot 
    {
        private:
            inverse_kinematics_model& kinematics;
            imotor3& motors;
            effector_boundaries* boundaries;

            point3 effector_location;
            bool boundaries_generated;

            bool is_valid_angle(double angle);
        
        public:
            /**
             * @brief constructor
             * @param kinematics kinematics model that will be used to convert points to motions
             * @param motors implementation of motor interface that will be used to communicate with the motors
             **/
            deltarobot(inverse_kinematics_model& kinematics, imotor3& motors);

            ~deltarobot(void);
            
            inline effector_boundaries* get_boundaries();
            inline bool has_boundaries();
            void generate_boundaries(double voxel_size);

            /**
             * @brief makes the deltarobot move to a point
             * @param p 3-dimensional point to move to
             * @param speed movement speed speed in degrees per second
             * @param async motions will be stored in a queue for later execution if true
             **/
            void moveto(const point3& p, double speed, bool async = true);

            /**
             * @brief stops the motors
             **/
            void stop(void);

            /**
             * @brief wait for the deltarobot to become idle
             *
             * the deltarobot is idle when all it's motions are completed
             * and it has stopped moving
             *
             * @param timeout time in milliseconds for the wait to timeout. 0 means infinite
             **/
            bool wait_for_idle(long timeout = 0);

            /**
             * @brief true if the deltarobot is idle, false otherwise
             **/
            bool is_idle(void);

            /**
             * @brief shuts down the deltarobot's hardware
             */
            void power_off(void);

            /**
             * @brief turns on the deltarobot's hardware
             */
            void power_on(void);
    };

    effector_boundaries* deltarobot::get_boundaries(){return boundaries;}
    bool deltarobot::has_boundaries(){return boundaries_generated;}
}
