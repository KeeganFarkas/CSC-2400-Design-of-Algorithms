/*
    Title:    kwfarkas42.cpp
    Author:   Keegan Farkas
    Date:     September 30, 2022
    Purpose:  To find the the points that lay on the convex hull of a set of points 
			  and display them in lexicographical order
*/
#include <algorithm>
#include <exception>
#include <fstream>
#include <iterator>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

#include <cerrno>
#include <cstring>

#include <sys/time.h>

/* Point.
 *
 * Points will be stored in a std::pair<double,double>. This typedef has been added for convenience.
 * */
typedef std::pair<double,double> point;

/* Prints a point.
 *
 * This function is used to add a point to an ostream. The point is printed in the form "(pt.first,pt.second)".
 *
 * @param os - the ostream the point is copied to
 * @param pt - the point to copy to the ostream 
 * */
std::ostream & operator<<(std::ostream &os, const point &pt);

/* Prints a vector of points.
 *
 * This function is used to add a vector of points to an ostream. Each point in the vector is added to the stream and seperated using std::endl.
 *
 * @param os - the ostream the vector is copied to
 * @param pt - the vector to copy to the ostream
 * */
std::ostream & operator<<(std::ostream &os, const std::vector<point> &pts);

/* Line segement.
 *
 * Line segments will be stored in a std::pair<std::pair<double,double>,std::pair<double,double>>
 * which is also of type std::pair<point,point>. This typedef has been added for convenience.
 * */
typedef std::pair<point,point> line_segment;

/* Get the beginning of a line segment.
 *
 * This function returns the starting point of a line segment (i.e., sgmt.first).
 *
 * @param sgmt - the line segement
 * */
const point & get_first(const line_segment &sgmt);

/* Get the ending of a line segment.
 *
 * This function returns the end point of a line segment (i.e., sgmt.second).
 *
 * @param sgmt - the line segement
 * */
const point & get_second(const line_segment &sgmt);

/* Displays the useage message.
 *
 * This function displays the command line usage message on how to run the program.
 *
 * @param name - the name of the program executable
 * */
void usage(char *name);

/* Reads points in a file.
 *
 * This function reads points from filename and stores them in ret_pts. Each line of filename
 * is assumed to be of the form "x y" where x and y are doubles. Points are added to the end
 * of ret_pts and the original points are left intact. Duplicate points are found in the file
 * are ignored.
 *
 * @param filename - name of the file to read
 * @param ret_pts - container used to store points
 *
 * @throws std::runtime_error - thrown if there is an i/o error or conversion error
 *
 * */
void read_points(const std::string &filename, std::vector<point> &ret_pts);

/* Convex hull algorithm.
 *
 * This function calculates the convex hull of the pts vector. It is assumed that pts contain 2
 * or more points and that no point in pts will lie on the boundry of the convex polygon unless
 * they are vertices of said polygon. The line segments defining the convex polygon are added
 * to ret_sgmts.
 *
 * @parm pts - a vector of n>1 points for which to find the convex hull
 * @parm ret_sgmts - line segments composing the convex hull are added to this vector
 * */
void brute_force_convex_hull(const std::vector<point> &pts, std::vector<line_segment> &ret_sgmts);

/*
 * Finds the convex hull.
 *
 * This function is a wrapper to the brute_force_convex_hull function. It accepts a collection
 * of points and passes them to the brute_force_convex_hull function. The points comprising the
 * line segements returned by the brute_force_convex_hull are copied to the ret_pts vector. 
 *
 * @param pts - the points for which to find the convex hull
 * @param hull_pts - the vector the vertices of the convex hull are copied into
 * */
void find_hull(const std::vector<point> &pts, std::vector<point> &ret_hull_pts);






std::ostream & operator<<(std::ostream &os, const point &pt) {
	os << "(" << pt.first << "," << pt.second << ")";
	return os;
}

std::ostream & operator <<(std::ostream &os, const std::vector<point> &pts) {
	std::vector<point>::const_iterator iter = pts.begin();
	while(iter != pts.end()) {
		os << *iter;
		++iter;
		if(iter != pts.end()) {
			os << std::endl;
		}
	}
	return os;
}

const point & get_first(const line_segment &sgmt) {
	return sgmt.first;
}

const point & get_second(const line_segment &sgmt) {
	return sgmt.second;
}

void usage(char *name) {
	std::cout << "usage: ";
	std::cout << name << " infile" << std::endl;
	std::cout << "  infile - file containing points" << std::endl << std::endl;
	std::cout << "It is assumed that each line of <infile> contains" << std::endl; 
	std::cout << "a point of form x y where x and y are real numbers." << std::endl;
}

void read_points(const std::string &filename, std::vector<point> &ret_pts) {
	point pt;
	std::ifstream ifs(filename.c_str());
	if(ifs) {
		// read points into a set to remove duplicates
		std::set<point> pts;
		while(ifs >> pt.first && ifs >> pt.second) {
			pts.insert(pt);
		}

		// check if there was an error reading in the file
		if(ifs.bad()) { // i/o error
			std::ostringstream oss;
			oss << filename << ": " << strerror(errno);
			throw std::runtime_error(oss.str());
		}
		else if((ifs.fail() && !ifs.eof())) { // conversion error
			std::ostringstream oss;
			oss << filename << ": error reading point";
			throw std::runtime_error(oss.str());
		}

		// copy the points read to the end of the ret_pts vector
		std::copy(pts.begin(), pts.end(), std::back_inserter(ret_pts));
	}
	else { // error opening file
		std::ostringstream oss;
		oss << filename << ": " << strerror(errno);
		throw std::runtime_error(oss.str());
	}
}

void brute_force_convex_hull(const std::vector<point> &pts, std::vector<line_segment> &ret_sgmts) {
	// loops through all the points except the last point
	for(long long unsigned int i = 0; i < (pts.size() - 1); i++){

		// loops through all the points except the first point
		for(long long unsigned int j = i + 1; j < (pts.size()); j++){

			// finds a, b, and c for ax + by = c
			double a = pts[j].second - pts[i].second;
			double b = pts[i].first - pts[j].first;
			double c = (pts[j].second * pts[i].first) - (pts[i].second * pts[j].first);

			bool LT = false;
			bool GT = false;

			// loops through all the points
			for(long long unsigned int k = 0; k < pts.size(); k++){
				double val = (a * pts[k].first) + (b * pts[k].second) - c;

				// marks LT or GT true for which side of the line from i to j the current point k is on
				if(val < 0)
					LT = true;
				else if(val > 0)
					GT = true;
			}

			// adds the line from i to j to the vector if all points are on the same side
			if(LT != true || GT != true){
				line_segment line;
				line.first = pts[i];
				line.second = pts[j];
				ret_sgmts.push_back(line);
			}
		}
	}
}

void find_hull(const std::vector<point> &pts, std::vector<point> &ret_hull_points) {
	if(pts.size() == 0) {
		std::ostringstream oss;
		oss << "error: one or more points are required to find the convex hull";
		throw std::runtime_error(oss.str());
	}
	else if(pts.size() == 1) {
		// the convex hull of a single point is the point itself
		ret_hull_points.push_back(*pts.begin());	
	}
	else {
		/* https://stackoverflow.com/a/1041939 
		 * 
		 * We need to copy the endpoints of the line segments found in
		 * brute_force_convex_hull to ret_hull_points, but we don't want duplicate
		 * points. We will copy the points into a set to remove the duplicates
		 * and then copy the points to ret_hull_points.
		 * */
		std::set<point> pts_set;
		std::vector<line_segment> sgmts;
		brute_force_convex_hull(pts, sgmts);

		/* copy the first point of the line segments to pts_set */
		std::transform(
			sgmts.begin(),
			sgmts.end(),
			std::inserter(pts_set, pts_set.end()),
			get_first
		);
		/* copy the second point of the line segments to pts_set */
		std::transform(
			sgmts.begin(),
			sgmts.end(),
			std::inserter(pts_set, pts_set.end()),
			get_second
		);
		/* copy the points from pts_set to ret_hull_points */
		std::copy(pts_set.begin(), pts_set.end(), std::back_inserter(ret_hull_points));
	}
}

int main (int argc, char *argv[]) {
	if(argc != 2) {
		std::cerr << "Invalid number of arguments." << std::endl << std::endl;
		usage(argv[0]);
	}
	else {
		try {
			struct timeval start_tv, end_tv;
			std::string infile(argv[1]);
			std::vector<point> pts;
			std::vector<point> hull_pts;

			read_points(infile, pts);
			
			gettimeofday(&start_tv, 0);
			find_hull(pts, hull_pts);
			gettimeofday(&end_tv, 0);

			std::cout << "Convex Hull (" << hull_pts.size() << " Points):" << std::endl;
			std::cout << hull_pts << std::endl;
			std::cout << "Elapsed Time (microseconds): "
				<< (end_tv.tv_sec - start_tv.tv_sec)*1000000L + (end_tv.tv_usec - start_tv.tv_usec)
				<< std::endl;
		}
		catch (std::exception &ex) {
			std::cerr << ex.what() << std::endl;
		}
	}

	return 0;
}
