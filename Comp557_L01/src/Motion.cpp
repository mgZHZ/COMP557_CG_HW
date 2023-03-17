/**
 COMP557 Assignment1
 Hanzhi Zhang
 260908416
 */
#include "Motion.h"
#include "DAGNode.h"
#include "MatrixStack.h"

#include <iostream>
#include <cassert>

#include <cassert>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <limits>
#include <sstream>
#include <utility>

#include "GLSL.h"

#include "tiny_obj_loader.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace std;

Motion::Motion()
{
}

Motion::~Motion()
{
}

// replicated from tiny_obj_loader.h GROSS!  :(
// See
// http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
static std::istream& safeGetline(std::istream& is, std::string& t) {
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    if (se) {
        for (;;) {
            int c = sb->sbumpc();
            switch (c) {
            case '\n':
                return is;
            case '\r':
                if (sb->sgetc() == '\n') sb->sbumpc();
                return is;
            case EOF:
                // Also handle the case when the last line has no line ending
                if (t.empty()) is.setstate(std::ios::eofbit);
                return is;
            default:
                t += static_cast<char>(c);
            }
        }
    }

    return is;
}

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

static void tokenizeNextLine(std::ifstream& ifs, vector<string>& words) {
    std::string linebuf;
    safeGetline(ifs, linebuf);
    linebuf = ltrim(linebuf);
    std::string delimiter = " ";
    size_t pos = 0;
    std::string token;
    while ((pos = linebuf.find(delimiter)) != std::string::npos) {
        if (pos > 0) words.push_back(linebuf.substr(0, pos));
        linebuf.erase(0, pos + delimiter.length());
    }
    if (linebuf.size() > 0) {
        words.push_back(linebuf);
    }
}

static void getOffset(std::ifstream& ifs, glm::fvec3& offset ) {
    vector<string> words{};
    tokenizeNextLine(ifs, words);
    assert(words[0].compare("OFFSET") == 0);
    assert(words.size() == 4);
    offset.x = atof(words[1].c_str());
    offset.y = atof(words[2].c_str());
    offset.z = atof(words[3].c_str());
}

static void getChannels(std::ifstream& ifs, std::vector<std::string>& channels) {
    vector<string> words{};
    tokenizeNextLine(ifs, words);
    assert(words[0].compare("CHANNELS") == 0);
    int num = atoi(words[1].c_str());
    assert(words.size() == 2 + num);
    for (int i = 0; i < num; i++) {
        channels.push_back(words[2 + i]);
    }
}

bool Motion::loadBVH(const std::string& bvhName)
{
    std::ifstream ifs(bvhName);
    if (!ifs) {
        cerr << "Cannot open file [" << bvhName << "]" << std::endl;
        return false;
    }
    size_t line_num = 0;
    std::string linebuf;

    bool bReadingHierarchy = false;

    DAGNode* node = NULL; // current node to which we are addding children
    numChannels = 0; // for counting channels
    while (ifs.peek() != -1) {
        line_num++;

        safeGetline(ifs, linebuf);
        // Trim newline '\r\n' or '\n'
        if (linebuf.size() > 0) {
            if (linebuf[linebuf.size() - 1] == '\n')
                linebuf.erase(linebuf.size() - 1);
        }
        if (linebuf.size() > 0) {
            if (linebuf[linebuf.size() - 1] == '\r')
                linebuf.erase(linebuf.size() - 1);
        }
        linebuf = ltrim(linebuf);
        // Skip if empty line.
        if (linebuf.empty()) {
            continue;
        }
        // Skip leading space.
        const char* line = linebuf.c_str();
        line += strspn(line, " \t");

        assert(line);
        if (line[0] == '\0') continue;  // empty line (shouldn't be any)

        if (bReadingHierarchy) {
            std::string delimiter = " ";
            size_t pos = 0;
            std::string token;
            vector<string> words{};
            while ((pos = linebuf.find(delimiter)) != std::string::npos) {
                words.push_back(linebuf.substr(0, pos));
                linebuf.erase(0, pos + delimiter.length());
            }
            if (linebuf.size() > 0) {
                words.push_back(linebuf);
            }

            if ( words[0].compare("ROOT") == 0) {
                node = new DAGNode();
                root = node;
                node->parent = NULL;
                node->name = words[1];
                node->channelDataStartIndex = numChannels;

                // get the opening bracket
                safeGetline(ifs, linebuf);
                const char* line = linebuf.c_str(); // Skip leading space.
                line += strspn(line, " \t");
                assert(line[0]=='{');
                // get the offset
                getOffset(ifs, node->offset);
                // get the channels
                getChannels(ifs, node->channels);
                numChannels += node->channels.size();
            }
            else if (words[0].compare("}") == 0) {
                node = node->parent;
                if (node == NULL) {
                    bReadingHierarchy = false;
                }
            }
            else if (words[0].compare("JOINT") == 0) {
                DAGNode* newnode = new DAGNode();
                node->children.push_back(newnode);
                newnode->parent = node;
                newnode->name = words[1];
                node = newnode; // work with the new node
                node->channelDataStartIndex = numChannels;

                // get the opening bracket
                safeGetline(ifs, linebuf);
                const char* line = linebuf.c_str(); // Skip leading space.
                line += strspn(line, " \t");
                assert(line[0] == '{');
                // get the offset
                getOffset(ifs, node->offset);
                // get the channels
                getChannels(ifs, node->channels);
                numChannels += node->channels.size();
            }
            else if (words[0].compare("End") == 0) {
                // get the opening bracket
                safeGetline(ifs, linebuf);
                linebuf = ltrim(linebuf);
                assert(linebuf.compare("{") == 0);
                DAGNode* newnode = new DAGNode();
                node->children.push_back(newnode);
                newnode->parent = node;
                newnode->name = "EndSite";
                // get the offset
                getOffset(ifs, newnode->offset);
                safeGetline(ifs, linebuf);
                linebuf = ltrim(linebuf);
                assert(linebuf.compare("}") == 0);
            }
        } else {
            int val = strcmp(line, "HIERARCHY");
            if ( val == 0) {
                bReadingHierarchy = true;
            } 
            val = strcmp(line, "MOTION");
            if (val == 0) {
                vector<string> words{};

                tokenizeNextLine(ifs, words);
                assert(words.size() == 2);
                assert(words[0].compare("Frames:")==0);
                numFrames = stoi(words[1]);
                words.clear();
                tokenizeNextLine(ifs, words);
                assert(words.size() == 3);
                assert(words[0].compare("Frame")==0);
                assert(words[1].compare("Time:")==0);
                frameTime = stof(words[2]);

                data = new float[numFrames * numChannels];
                int c = 0;
                for (int i = 0; i < numFrames; i++) {
                    words.clear();
                    tokenizeNextLine(ifs, words);
                    assert(words.size() == numChannels);
                    for (int j = 0; j < numChannels; j++) {
                        data[c++] = stof(words[j]);
                    }
                }
            }
        }
    }
    return true;
}
