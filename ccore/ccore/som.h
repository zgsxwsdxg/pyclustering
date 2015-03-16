/**************************************************************************************************************

Neural Network: Self-Organized Feature Map

Based on article description:
 - T.Kohonen. The Self-Organizing Map. 1990.
 - T.Kohonen, E.Oja, O.Simula, A.Visa, J.Kangas. Engineering Applications of the Self-Organizing Map. 1996.

Copyright (C) 2015    Andrei Novikov (spb.andr@yandex.ru)

pyclustering is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

pyclustering is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************************************************************/

#ifndef _SOM_H_
#define _SOM_H_

#include <vector>

typedef enum som_conn_type {
	SOM_GRID_FOUR,
	SOM_GRID_EIGHT,
	SOM_HONEYCOMB,
	SOM_FUNC_NEIGHBOR
} som_conn_type;

typedef enum som_init_type {
	SOM_RANDOM,
	SOM_RANDOM_CENTROID,
	SOM_RANDOM_SURFACE,
	SOM_UNIFORM_GRID
} som_init_type;

typedef struct som_parameters {
	som_init_type init_type;
	double init_radius;
	double init_learn_rate;
	double adaptation_threshold;

	som_parameters() : 
		init_type(SOM_UNIFORM_GRID), 
		init_radius(1.0), 
		init_learn_rate(0.1), 
		adaptation_threshold(0.001) { }
} som_parameters;

/***********************************************************************************************
 *
 * @brief   Self-Orzanized Feature Map based on Kohonen desription of SOM.
 *
 ***********************************************************************************************/
class som {
private:
	/* network description */
	unsigned int rows;
	unsigned int cols;
	unsigned int size;

	som_conn_type conn_type;

	std::vector<std::vector<double> * > * weights;
	std::vector<std::vector<double> * > * previous_weights;
	std::vector<unsigned int> * awards;

	std::vector<std::vector<double> > * data;

	/* just for convenience (avoid excess calculation during learning) */
	std::vector<std::vector<double> * > * location;
	std::vector<std::vector<double> * > *  sqrt_distances;
	std::vector<std::vector<unsigned int> * > * capture_objects;
	std::vector<std::vector<unsigned int> * > * neighbors;

	/* describe learning process and internal state */
	unsigned int epouchs;
	som_parameters params;

	/* dynamic changes learning parameters */
	double local_radius;
	double learn_rate;

public:
	/***********************************************************************************************
	 *
	 * @brief   Constructor of self-organized map.
	 *
	 * @param   (in) data            - list of points where each point is represented by list of features, 
	 *                                 for example coordinates.
	 * @param   (in) rows            - number of neurons in the column (number of rows).
	 * @param   (in) cols            - number of neurons in the row (number of columns).
	 * @param   (in) epochs          - number of epochs for training.
	 * @param   (in) conn_type       - type of connection between oscillators in the network.
	 * @param   (in) parameters		 - others parameters of the network.
	 *
	 ***********************************************************************************************/
	som(std::vector<std::vector<double> > * input_data, const unsigned int num_rows, const unsigned int num_cols, const unsigned int num_epochs, const som_conn_type type_conn, const som_parameters * parameters);

	/***********************************************************************************************
	 *
	 * @brief   Default destructor.
	 *
	 ***********************************************************************************************/
	~som(void);

	/***********************************************************************************************
	 *
	 * @brief   Trains self-organized feature map (SOM).
	 *
	 * @param   (in) index_winner    - index of neuron-winner.
	 * @param   (in) pattern         - input pattern from the input data set.
	 *
	 * @return  Returns number of learining iterations.
	 *
	 ***********************************************************************************************/
	unsigned int train(bool autostop);

	/***********************************************************************************************
	 *
	 * @brief   Processes input pattern (no learining) and returns index of neuron-winner.
	 *          Using index of neuron winner catched object can be obtained by get_capture_objects().
	 *
	 * @param   (in) pattern    - input pattern.
	 *
	 * @return  Returns index of neuron-winner.
	 *
	 ***********************************************************************************************/
	unsigned int simulate(const std::vector<double> * pattern) const;

	/***********************************************************************************************
	 *
	 * @return  Returns number of winner at the last step of learning process.
	 *
	 ***********************************************************************************************/
	unsigned int get_winner_number(void) const;

	/***********************************************************************************************
	 *
	 * @return  Returns size of self-organized map (number of neurons).
	 *
	 ***********************************************************************************************/
	inline unsigned int get_size(void) const { return size; }

	/***********************************************************************************************
	 *
	 * @return  Returns list of weights of each neuron.
	 *
	 ***********************************************************************************************/
	inline const std::vector<std::vector<double> * > * const get_weights(void) const { return weights; }

	/***********************************************************************************************
	 *
	 * @return  Returns list of indexes of captured objects by each neuron.
	 *
	 ***********************************************************************************************/
	inline const std::vector<std::vector<unsigned int> * > * const get_capture_objects(void) const { return capture_objects; }

	/***********************************************************************************************
	 *
	 * @return  Returns list of indexes of neighbors of each neuron.
	 *
	 ***********************************************************************************************/
	inline const std::vector<std::vector<unsigned int> * > * const get_neighbors(void) const { return neighbors; }
	
	/***********************************************************************************************
	 *
	 * @return  Returns list of numbers of captured objects by each neuron.
	 *
	 ***********************************************************************************************/
	inline const std::vector<unsigned int> * const get_awards(void) const { return awards; }

private:
	/***********************************************************************************************
	 *
	 * @brief   Create connections in line with input rule (grid four, grid eight, honeycomb, 
	 *          function neighbour).
	 *
	 * @param   (in) conn_type       - type of connection between oscillators in the network. 
	 *
	 ***********************************************************************************************/
	void create_connections(const som_conn_type type_conn);

	/***********************************************************************************************
	 *
	 * @brief   Creates initial weights for neurons in line with the specified initialization.
	 *
	 * @param   (in) init_type       - type of initialization of initial neuron weights (random, 
	 *                                 random in center of the input data, random distributed in 
	 *                                 data, ditributed in line with uniform grid). 
	 *
	 ***********************************************************************************************/
	void create_initial_weights(const som_init_type type_init);

	/***********************************************************************************************
	 *
	 * @brief   Returns neuron winner (distance, neuron index).
	 *
	 * @param   (in) pattern    - input pattern from the input data set, for example it can be 
	 *                            coordinates of point.
	 *
	 * @return  Returns index of neuron that is winner.
	 *
	 ***********************************************************************************************/
	unsigned int competition(const std::vector<double> * pattern) const;

	/***********************************************************************************************
	 *
	 * @brief   Change weight of neurons in line with won neuron.
	 *
	 * @param   (in) index_winner    - index of neuron-winner.
	 * @param   (in) pattern         - input pattern from the input data set.
	 *
	 ***********************************************************************************************/
	unsigned int adaptation(const unsigned int index_winner, const std::vector<double> * pattern);

	/***********************************************************************************************
	 *
	 * @brief   Returns maximum changes of weight in line with comparison between previous weights 
	 *          and current weights.
	 *
	 * @return  Returns value that represents maximum changes of weight after adaptation process.
	 *
	 ***********************************************************************************************/
	double calculate_maximal_adaptation(void) const;
};

#endif
