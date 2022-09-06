/* Image Processing Module
 * (Quicksort Analysis) 
 * 1. Performs well when there is a lot of data (more frames)
 * 2. Optimally this should be running for a couple of minutes to ensure reliable results according to Yan et. al. (Real-time Driver Drowsiness Detection System Based on PERCLOS and Grayscale Image Processing). 
 * Should it run less than the optimal duration, it may still perform well assuming the person provides adequate data for open and closed states. That is, the person is blinking frequently. */

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

double _averageDuration = 0; // average duration (averageDuration)
int _fetchedClock = 0; // gets lClock() at 1s (averageFps)
double _averageFps = 0; // average fps (averageFps)
double _frameNo = 0; // no of frames in 1s (averageFps)

/* Clock */
int lClock() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	// cout << "lClock: " << (ts.tv_sec * 1000) + (ts.tv_nsec * 1e-6) << endl; 
	return (ts.tv_sec * 1000) + (ts.tv_nsec * 1e-6);
}

/* Average Duration */
double averageDuration(double newDuration) {
	_averageDuration = 0.98 * _averageDuration + 0.02 * newDuration;
	return _averageDuration;
}	

/* Average fps */
double averageFps() {
	if ((lClock() - _fetchedClock) > 1000) {
		_fetchedClock = lClock();
		_averageFps = 0.7 * _averageFps + 0.3 * _frameNo;
		// cout << "fps: " << _frameNo << endl;
		_frameNo = 0;
	}
	_frameNo++;
	return _averageFps;
}

/* Partition */
int partition(float arr[], int start, int end) {
    float pivot = arr[start];
    int count = 0;
    for (int i = start + 1; i <= end; i++) {
        if (arr[i] <= pivot)
            count++;
    }
    
    /* Giving pivot element its correct position */
    int pivotIndex = start + count;
    swap(arr[pivotIndex], arr[start]);

    /* Sorting left and right parts of the pivot element */
    int i = start, j = end;
    while (i < pivotIndex && j > pivotIndex) {
 
        while (arr[i] <= pivot) {
            i++;
        }
        while (arr[j] > pivot) {
            j--;
        }
        if (i < pivotIndex && j > pivotIndex) {
            swap(arr[i++], arr[j--]);
        }
    }
    return pivotIndex;
}

/* Quicksort */
void quickSort(float arr[], int start, int end) {
    if (start >= end) return; // base case
    int p = partition(arr, start, end); // partitioning the array
    quickSort(arr, start, p - 1); // Sorting the left part
    quickSort(arr, p + 1, end); // Sorting the right part
}

/* Rotate Array */
void Rotate(float arr[], int d, int n)
{
    int p = 1;
    while (p <= d) {
        int last = arr[0];
        for (int i = 0; i < n - 1; i++) {
            arr[i] = arr[i + 1];
        }
        arr[n - 1] = last;
        p++;
    }
}

int main() {
	/* Camera */
	// VideoCapture cap(0); // default cam
	Mat crop, gray, blur, thresh;
	int minThresh = 50; // for thresholding
	int maxThresh = 255; // for thresholding
	int frameNo = 0;

	/* Video */
	// string path = "vid/jems-stabilized.mp4"; // video path
	// string path = "vid/jun-stabilized.mp4"; // video path
	// string path = "vid/mitcham-stabilized.mp4"; // video path
	string path = "vid/norman-stabilized.mp4"; // video path
	// string path = "vid/rhys-stabilized.mp4"; // video path
	VideoCapture cap(path);
	Mat frame;
	int counter = 0;
	int fetchedClock = 0;


	/* Blink Util */
	int windowSize = 1925; // 5s window (100fps * 5s) (average 1 blink every 5s)
	// int windowSize = 1800; // 60s window (30fps * 60s)
	float framePixels[1925] = {2300, 2300, 2297, 2300, 2300, 2285, 2285, 2285, 2288, 2291, 2291, 2290, 2290, 2298, 2298, 2297, 2294, 2296, 2302, 2302, 2301, 2301, 2280, 2285, 2285, 2285, 2278, 2277, 2277, 2278, 2276, 2276, 2277, 2277, 2276, 2272, 2272, 2272, 2271, 2292, 2292, 2292, 2292, 2285, 2285, 2286, 2285, 2307, 2307, 2307, 2307, 2305, 2305, 2305, 2305, 2303, 2308, 2306, 2305, 2305, 2305, 2297, 2297, 2299, 2311, 2266, 2265, 2265, 2266, 1740, 1740, 1740, 1740, 1740, 1743, 1743, 1743, 1743, 1648, 1646, 1648, 1846, 1846, 1846, 1846, 2031, 2031, 2031, 2031, 2142, 2142, 2142, 2143, 2221, 2221, 2221, 2221, 2270, 2270, 2271, 2274, 2274, 2274, 2274, 2287, 2292, 2294, 2293, 2294, 2269, 2268, 2269, 2258, 2260, 2260, 2254, 2254, 2232, 2232, 2231, 2222, 2227, 2227, 2219, 2219, 2219, 2225, 2225, 2224, 2231, 2232, 2187, 2187, 2188, 2188, 2188, 2184, 2170, 2170, 2170, 2195, 2195, 2181, 2182, 2216, 2213, 2213, 2213, 2222, 2222, 2217, 2219, 2225, 2015, 2015, 1493, 1493, 1493, 1493, 1579, 1579, 1578, 1579, 1598, 1597, 1599, 1596, 1923, 1923, 1923, 1923, 2187, 2187, 2186, 2187, 2264, 2263, 2261, 2314, 2314, 2314, 2312, 2315, 2314, 2300, 2295, 2297, 2304, 2302, 2301, 2301, 2301, 2294, 2301, 2302, 2295, 2295, 2297, 2297, 2269, 2296, 2297, 2296, 2282, 2282, 2284, 2284, 2275, 2271, 2272, 2274, 2247, 2247, 2247, 2246, 2247, 2245, 2247, 2246, 2239, 2239, 2235, 2239, 2180, 2179, 2181, 2181, 1582, 1580, 1580, 1575, 1575, 1575, 1575, 1575, 1580, 1580, 1580, 1579, 2018, 2018, 2017, 2013, 2183, 2183, 2183, 2183, 2234, 2251, 2234, 2234, 2241, 2240, 2240, 2241, 2241, 2267, 2242, 2242, 2249, 2249, 2249, 2248, 2238, 2265, 2241, 2241, 2241, 2229, 2229, 2229, 2229, 2238, 2234, 2234, 2235, 2230, 2230, 2229, 2230, 2231, 2242, 2242, 2241, 2241, 2226, 2226, 2226, 2226, 2236, 2237, 2237, 2237, 2237, 2223, 2222, 2219, 2224, 2207, 2209, 2208, 2208, 1766, 1766, 1766, 1764, 1470, 1468, 1468, 1467, 1470, 1670, 1670, 1670, 1670, 1970, 1969, 1969, 1969, 1969, 1969, 1969, 1970, 1970, 2155, 2152, 2156, 2168, 2168, 2154, 2154, 2278, 2278, 2277, 2279, 2279, 2288, 2278, 2287, 2276, 2281, 2283, 2283, 2277, 2286, 2293, 2293, 2292, 2286, 2294, 2294, 2293, 2294, 2296, 2296, 2307, 2307, 2307, 2307, 2306, 2297, 2297, 2296, 2298, 2298, 2293, 2294, 2293, 2288, 2291, 2319, 2319, 2319, 2319, 2297, 2297, 2298, 2298, 2297, 2139, 2138, 2138, 2136, 1316, 1317, 1315, 1315, 1315, 1776, 1776, 1778, 1778, 2064, 2064, 2063, 2062, 2062, 2202, 2198, 2199, 2199, 2203, 2267, 2267, 2267, 2265, 2270, 2270, 2271, 2270, 2272, 2269, 2271, 2281, 2281, 2279, 2275, 2275, 2275, 2275, 2277, 2279, 2279, 2281, 2283, 2283, 2283, 2283, 2280, 2262, 2263, 2263, 2263, 2263, 2281, 2282, 2282, 2281, 2281, 2257, 2259, 2260, 2260, 2261, 2262, 2266, 2268, 2272, 2265, 2077, 2077, 2077, 2078, 2077, 2077, 1665, 1665, 1665, 1665, 1665, 1676, 1676, 1676, 1675, 1675, 1858, 1858, 1856, 1856, 2156, 2156, 2156, 2156, 2156, 2234, 2234, 2234, 2234, 2234, 2262, 2259, 2260, 2261, 2263, 2283, 2283, 2280, 2283, 2287, 2287, 2287, 2289, 2289, 2288, 2285, 2288, 2286, 2263, 2264, 2264, 2264, 2264, 2271, 2272, 2269, 2269, 2269, 2269, 2262, 2260, 2258, 2258, 2282, 2283, 2283, 2283, 2283, 2283, 2282, 2286, 2286, 2284, 2290, 2290, 2290, 2290, 2290, 2294, 2286, 2278, 2278, 2278, 2278, 2278, 2103, 2103, 2104, 2103, 2103, 1729, 1728, 1728, 1728, 1728, 1830, 1830, 1830, 1830, 2000, 2000, 2000, 2000, 2000, 2000, 2172, 2172, 2172, 2172, 2287, 2287, 2290, 2330, 2330, 2330, 2330, 2330, 2348, 2343, 2347, 2347, 2347, 2347, 2358, 2358, 2358, 2361, 2353, 2353, 2353, 2353, 2353, 2351, 2365, 2354, 2354, 2354, 2354, 2372, 2372, 2372, 2373, 2366, 2366, 2372, 2370, 2371, 2369, 2371, 2371, 2374, 2368, 2368, 2282, 2277, 2277, 2278, 2278, 2283, 2274, 2264, 2263, 2264, 2263, 2263, 2264, 2350, 2348, 2347, 2346, 2349, 2345, 1499, 1501, 1499, 1499, 1499, 1499, 1676, 1676, 1674, 1672, 1673, 1673, 1831, 1830, 1833, 1830, 1831, 2067, 2067, 2067, 2067, 2178, 2177, 2178, 2177, 2177, 2187, 2213, 2213, 2212, 2214, 2214, 2215, 2215, 2235, 2244, 2245, 2245, 2246, 2170, 2173, 2173, 2180, 2168, 2172, 2170, 2170, 2174, 2166, 2229, 2226, 2234, 2234, 2231, 2232, 2232, 2228, 2228, 1507, 1506, 1506, 1506, 1506, 1509, 1507, 1506, 1494, 1494, 1491, 1494, 1963, 1964, 1963, 1962, 1963, 2249, 2249, 2248, 2248, 2242, 2247, 2407, 2407, 2406, 2408, 2410, 2498, 2482, 2481, 2481, 2481, 2491, 2484, 2464, 2472, 2464, 2465, 2463, 2470, 2470, 2447, 2441, 2443, 2441, 2441, 2444, 2444, 2443, 2444, 2444, 2444, 2447, 2421, 2419, 2419, 2419, 2419, 2413, 2417, 2407, 2407, 2405, 2408, 2422, 2422, 2420, 2422, 2422, 1616, 1616, 1616, 1616, 1517, 1517, 1517, 1517, 1517, 1517, 1719, 1719, 1719, 1897, 1897, 1897, 1897, 2089, 2089, 2097, 2099, 2098, 2225, 2221, 2227, 2227, 2357, 2355, 2354, 2353, 2359, 2423, 2423, 2424, 2422, 2422, 2420, 2418, 2438, 2415, 2415, 2414, 2417, 2417, 2464, 2460, 2476, 2476, 2466, 2498, 2498, 2498, 2498, 2498, 2500, 2496, 2496, 2495, 2495, 2496, 2474, 2472, 2472, 2473, 2479, 2479, 2479, 2480, 2480, 2477, 2480, 2479, 2480, 2479, 2280, 2280, 2280, 2280, 1807, 1807, 1807, 1809, 1807, 1807, 1771, 1768, 1768, 1769, 1769, 2133, 2133, 2132, 2132, 2132, 2131, 2131, 2284, 2283, 2283, 2284, 2283, 2337, 2337, 2333, 2337, 2335, 2339, 2335, 2335, 2346, 2347, 2347, 2347, 2339, 2340, 2340, 2340, 2340, 2343, 2343, 2343, 2343, 2343, 2340, 2308, 2308, 2304, 2308, 2308, 2295, 2295, 2295, 2295, 2295, 2279, 2300, 2300, 2293, 2299, 2295, 2297, 2300, 2289, 2289, 2285, 2284, 2284, 2282, 2282, 2282, 2292, 2286, 2278, 2279, 2276, 2278, 2275, 2275, 2275, 2276, 2279, 2277, 2280, 2283, 2278, 2277, 2278, 2278, 2273, 2274, 2276, 2272, 2276, 2275, 1762, 1761, 1754, 1763, 1763, 1763, 1790, 1790, 1791, 1791, 1792, 1793, 1789, 1788, 1788, 1787, 1788, 1788, 1788, 1789, 2222, 2222, 2221, 2221, 2231, 2230, 2231, 2337, 2338, 2338, 2337, 2338, 2338, 2330, 2332, 2476, 2477, 2476, 2481, 2480, 2475, 2475, 2475, 2471, 2471, 2495, 2492, 2492, 2491, 2503, 2503, 2504, 2503, 2503, 2397, 2399, 2398, 2397, 2356, 2357, 2355, 2355, 2364, 2365, 2365, 2364, 2358, 2349, 2349, 2349, 2349, 2349, 2340, 2351, 1690, 1691, 1693, 1692, 1692, 1710, 1710, 1709, 1709, 1708, 1707, 1711, 1872, 1869, 1874, 1868, 1871, 2102, 2102, 2100, 2102, 2217, 2217, 2219, 2221, 2220, 2219, 2221, 2218, 2219, 2220, 2213, 2216, 2222, 2219, 2224, 2224, 2217, 2222, 2190, 2190, 2190, 2190, 2190, 2190, 2143, 2154, 2145, 2145, 2145, 2146, 2148, 2146, 2146, 2157, 2155, 2157, 2157, 1428, 1430, 1430, 1430, 1430, 1431, 1432, 1640, 1640, 1639, 1640, 1887, 1888, 1888, 1888, 1888, 2112, 2112, 2111, 2111, 2207, 2201, 2204, 2204, 2206, 2207, 2207, 2205, 2249, 2249, 2247, 2250, 2251, 2247, 2249, 2250, 2212, 2207, 2208, 2208, 2209, 2189, 2190, 2191, 2191, 2165, 2165, 2165, 2156, 2164, 2167, 2167, 2167, 2165, 2163, 2170, 2165, 2167, 2167, 2142, 2142, 2142, 2142, 1736, 1736, 1736, 1736, 1736, 1736, 1736, 1736, 1736, 1736, 1735, 1692, 1688, 1693, 1693, 1691, 1692, 1692, 1693, 1690, 1689, 1691, 1690, 1690, 1691, 1692, 1690, 1691, 1690, 1690, 2030, 2030, 2030, 2030, 2030, 1658, 1658, 1658, 1658, 1659, 2098, 2109, 2106, 2108, 2221, 2220, 2223, 2242, 2245, 2272, 2283, 2292, 2278, 2290, 2294, 2281, 2278, 2292, 2273, 2272, 2271, 2272, 2272, 2256, 2256, 2252, 2258, 2258, 2258, 2242, 2242, 2242, 2241, 2224, 2224, 2224, 2224, 2224, 2228, 2228, 2228, 2216, 2242, 2242, 2242, 2242, 2242, 2232, 2230, 2230, 2233, 2230, 2231, 2230, 2231, 2191, 2191, 2186, 2191, 2189, 1501, 1501, 1501, 1501, 1501, 1462, 1462, 1462, 1462, 1634, 1634, 1631, 1634, 1634, 1634, 1633, 1633, 1983, 1985, 1985, 1983, 1983, 2264, 2264, 2264, 2264, 2263, 2303, 2303, 2304, 2303, 2303, 2330, 2318, 2318, 2316, 2318, 2298, 2298, 2298, 2303, 2298, 2268, 2267, 2267, 2267, 2260, 2260, 2258, 2258, 2260, 2239, 2239, 2239, 2239, 2239, 2257, 2261, 2261, 2261, 2253, 2253, 2251, 2253, 2246, 2251, 2252, 2252, 2273, 2273, 2273, 2273, 2273, 1702, 1702, 1700, 1700, 1706, 1706, 1705, 1706, 1841, 1841, 1841, 1841, 1841, 2131, 2131, 2131, 2130, 2235, 2235, 2234, 2235, 2266, 2259, 2268, 2267, 2252, 2256, 2256, 2251, 2243, 2240, 2244, 2242, 2226, 2226, 2227, 2227, 2234, 2234, 2223, 2231, 2208, 2215, 2210, 2204, 2202, 2193, 2204, 2205, 2173, 2173, 2173, 2176, 2177, 2174, 2174, 2174, 2174, 2166, 2167, 2167, 2169, 2169, 2142, 2139, 2142, 2142, 1524, 1524, 1524, 1524, 1524, 1524, 1330, 1330, 1330, 1330, 1489, 1488, 1489, 1488, 1718, 1718, 1718, 1718, 1908, 1908, 1908, 2100, 2099, 2099, 2103, 2102, 2103, 2079, 2079, 2079, 2096, 2096, 2112, 2112, 2115, 2115, 2117, 2142, 2142, 2142, 2142, 2142, 2196, 2196, 2196, 2196, 2143, 2143, 2143, 2143, 2162, 2162, 2162, 2162, 2145, 2145, 2145, 2145, 2134, 2139, 2135, 2134, 2134, 2079, 2077, 2077, 2079, 1596, 1596, 1596, 1596, 1596, 1379, 1379, 1379, 1379, 1670, 1670, 1669, 1668, 1669, 1880, 1882, 1882, 1881, 1883, 1883, 2185, 2185, 2186, 2185, 2249, 2249, 2250, 2249, 2248, 2244, 2246, 2260, 2258, 2261, 2260, 2260, 2260, 2252, 2254, 2254, 2254, 2254, 2250, 2286, 2286, 2286, 2286, 2333, 2333, 2318, 2332, 2332, 2310, 2310, 2310, 2310, 2311, 1844, 1844, 1844, 1844, 1808, 1805, 1807, 1807, 1807, 1758, 1758, 1760, 1759, 1759, 2046, 2046, 2046, 2046, 2046, 2264, 2264, 2263, 2264, 2263, 2334, 2336, 2339, 2335, 2339, 2343, 2343, 2341, 2356, 2338, 2385, 2388, 2385, 2385, 2361, 2359, 2367, 2362, 2361, 2347, 2345, 2344, 2344, 2347, 2339, 2341, 2340, 2337, 2337, 2322, 2319, 2316, 2323, 2319, 2317, 2318, 2317, 2317, 2301, 2301, 2302, 2300, 2300, 2287, 2286, 2287, 2287, 2247, 2250, 2244, 2249, 2245, 2254, 2254, 2254, 2254, 1628, 1628, 1628, 1628, 1631, 1632, 1559, 1558, 1558, 1560, 1810, 1807, 1809, 1807, 1810, 1982, 1982, 1982, 1982, 2134, 2138, 2134, 2134, 2132, 2243, 2238, 2248, 2247, 2247, 2270, 2269, 2269, 2312, 2294, 2287, 2286, 2286, 2297, 2294, 2294, 2298, 2283, 2283, 2283, 2283, 2284, 2273, 2266, 2288, 2268, 2268, 2270, 2270, 2270, 2270, 2219, 2221, 2220, 2220, 2220, 2203, 2203, 2204, 2205, 2202, 1825, 1824, 1822, 1821, 1821, 1533, 1530, 1537, 1536, 1536, 1691, 1687, 1684, 1685, 1687, 1906, 1903, 1902, 1902, 2122, 2120, 2120, 2124, 2180, 2178, 2173, 2180, 2210, 2205, 2221, 2210, 2209, 2205, 2205, 2205, 2204, 2204, 2215, 2209, 2209, 2208, 2146, 2146, 2149, 2149, 2125, 2126, 2124, 2121, 2125, 2130, 2130, 2129, 2117, 2131, 2157, 2157, 2157, 2156, 2161, 2161, 2159, 2159, 2155, 2154, 2154, 2151, 2153, 2140, 2140, 2138, 2142, 2156, 2145, 2146, 2146, 2159, 2138, 2132, 2137, 2136, 2136, 1652, 1654, 1651, 1653, 1658, 1656, 1760, 1760, 1760, 1760, 1760, 1820, 1820, 1819, 1825, 1824, 1822, 1925, 1926, 1926, 1923, 2056, 2051, 2051, 2053, 2103, 2103, 2101, 2101, 2136, 2140, 2136, 2136, 2151, 2151, 2153, 2149, 2174, 2172, 2174, 2170, 2171, 2165, 2171, 2169, 2165, 2167, 2201, 2198, 2198, 2198, 2198, 2207, 2210, 2205, 2207, 2156, 2156, 2156, 2144, 2157, 2156, 2162, 2155, 2151, 2153, 2021, 2017, 2016, 2023, 1727, 1727, 1726, 1728, 1622, 1622, 1622, 1622, 1618, 1671, 1672, 1677, 1673, 1673, 2028, 2029, 2027, 2022, 2022, 2182, 2182, 2183, 2183, 2204, 2200, 2200, 2187, 2188, 2188, 2193, 2188, 2202, 2200, 2200, 2200, 2199, 2182, 2184, 2182, 2182, 2182, 2211, 2210, 2210, 2210, 2203, 2215, 2217, 2215, 2216, 2215, 2217, 2213, 2212, 2212, 2212, 2213, 2205, 2205, 2204, 2204, 2204, 2207, 2203, 2201, 1705, 1706, 1706, 1706, 1700, 1700, 1704, 1702, 1677, 1676, 1676, 1676, 1807, 1805, 1807, 1808, 1961, 1961, 1961, 1961, 2134, 2130, 2131, 2132, 2212, 2210, 2213, 2213, 2232, 2221, 2230, 2229, 2242, 2242, 2242, 2241, 2266, 2260, 2260, 2257, 2281, 2281, 2281, 2281, 2255, 2256, 2258, 2258, 2226, 2223, 2224, 2224, 2230, 2224, 2223, 2221, 2221, 2220, 2223, 2234, 2233, 2234, 2236, 2220, 2218, 2218, 2221, 2189, 2190, 2190, 2190, 2190, 2223, 2220, 2222, 2222, 2218, 2213, 2214, 2212, 2212};
	float sortedFramePixels[windowSize];
	int count = windowSize;
	int close = 0;
	int open = 0;
	int temp_counter = 0;
	
	while(true) {
		clock_t start = lClock(); // start counting
		cap.read(frame);
		/* Print Array Once Full (Ends Program)*/
		/* if (frame.empty() || count == 1925) {
			cout << " Window Size: " << count << endl;
			cout << "{";
			for (const int &n : framePixels) {
				cout << n << ", ";
			}
			cout << "}";
			break;
		}  */
		if (frame.empty()) break;
	
		/* Process image */
		// crop = frame(Rect(170, 180, 230, 140)); // crop frame
		cvtColor(frame, gray, COLOR_BGR2GRAY); // convert to grayscale
		imshow("Gray", gray);
		GaussianBlur(gray, blur, Size(9, 9), 0); // apply gaussian blur
		threshold(blur, thresh, minThresh, maxThresh, THRESH_BINARY); // apply thresholding
		imshow("Threshold", thresh); // display window
		
		/* Calculate Histogram */
		MatND histogram;
		int histSize = 256;
		const int* channelNumbers = { 0 };
		float channelRange[] = { 0.0, 256.0 };
		const float* channelRanges = channelRange;
		int numberBins = 256;
		
		int histWidth = 512;
		int histHeight = 400;
		int binWidth = cvRound((double)histWidth / histSize);
		
		calcHist(&thresh, 1, 0, Mat(), histogram, 1, &numberBins, &channelRanges); // calculate
		
		/* Store pixels per frame */
		framePixels[count] = histogram.at<float>(0);
		count++;
		if (count == windowSize + 1) {
			count = windowSize - 2;
			
			/* SHL Array */
			Rotate(framePixels, 1, windowSize);

			/* Quicksort */
			for (int i = 0; i < windowSize; i++) {
            	sortedFramePixels[i] = framePixels[i];
			}
			quickSort(sortedFramePixels, 0, windowSize - 1);
			
			/* Get Average Number of Pixels (Open State) */
			int openEnd = windowSize - ((double)windowSize / 20); 
			int openStart = windowSize - ((double)windowSize / 10);  
			float sumOpen = 0.0;
			int openStates = 0;
			for (int i = openStart; i <= openEnd; i++) {
				sumOpen += sortedFramePixels[i];
			}
			openStates = sumOpen / (windowSize / 20);
			// cout << "Avg Open State Px.: " << openStates << endl;
			
			/* Get Average Number of Pixels (Closed State) */
			int closeEnd = windowSize / 10;
			int closeStart = windowSize / 20;
			float sumClose = 0.0;
			int closeStates = 0;
			for (int i = closeStart; i <= closeEnd; i++) {
				sumClose += sortedFramePixels[i];
			}
			closeStates = sumClose / (windowSize / 20);
			// cout << "Avg. Closed State Px.: " << closeStates << endl; 

			/* Threshold P80 */
			float threshold = (openStates - closeStates) * 0.2 +  closeStates;

			/* Separate Open from Closed State */
			int close_old = close;
			int open_old = open;
			close = 0;
			open = 0;
			for (int i = 0; i < windowSize; i++) {
				if (framePixels[i] < threshold) {
					close++;
				} else {
					open++;
				}
			}
			
			/* Print if blink */
			if (close_old < close) {
				cout << "(Close)";
				cout << " Counter: " << counter++;
				cout << endl;
			}

			// cout << "Thresh: " << threshold;
			cout << " Avg Open Px: " << openStates;
			cout << " Avg Close Px: " << closeStates;
			// cout << "(Close) No. of Frames: " << close;
			// cout << " Open: " << open;
			// cout << " temp_counter: " << temp_counter;
			// cout << " Prev. Frame - Curr. Frame: " << close_old - close;
			cout << endl;

			break;
			
		}
		averageFps();
		
		/* Exit at esc key */
		if (waitKey(1) == 27) {
            cout << "Program terminated." << endl;
            break;
        }
	}
	
	return 0;
}
