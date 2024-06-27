#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <ctime>

struct Particle {
    std::vector<double> position;
    std::vector<double> velocity;
    std::vector<double> bestPosition;
    double bestFitness;
};

double fitnessFunction(const std::vector<double>& position) {
    double fitness = 0.0;
    for (double x : position) {
        fitness += x * x;
    }
    return fitness;
}


void initializeParticle(Particle& particle, int dimensions, double minPos, double maxPos, double minVel, double maxVel) {
    particle.position.resize(dimensions);
    particle.velocity.resize(dimensions);
    particle.bestPosition.resize(dimensions);
    for (int i = 0; i < dimensions; ++i) {
        particle.position[i] = minPos + static_cast<double>(rand()) / RAND_MAX * (maxPos - minPos);
        particle.velocity[i] = minVel + static_cast<double>(rand()) / RAND_MAX * (maxVel - minVel);
    }
    particle.bestPosition = particle.position;
    particle.bestFitness = fitnessFunction(particle.position);
}


void updateParticle(Particle& particle, const std::vector<double>& globalBestPosition, double w, double c1, double c2) {
    int dimensions = particle.position.size();
    for (int i = 0; i < dimensions; ++i) {
        double r1 = static_cast<double>(rand()) / RAND_MAX;
        double r2 = static_cast<double>(rand()) / RAND_MAX;
        particle.velocity[i] = w * particle.velocity[i] + c1 * r1 * (particle.bestPosition[i] - particle.position[i]) + c2 * r2 * (globalBestPosition[i] - particle.position[i]);
        particle.position[i] += particle.velocity[i];
    }
    double currentFitness = fitnessFunction(particle.position);
    if (currentFitness < particle.bestFitness) {
        particle.bestFitness = currentFitness;
        particle.bestPosition = particle.position;
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    int dimensions = 30;
    int particleCount = 100;
    int maxIter = 1000;
    double minPos = -10.0, maxPos = 10.0;
    double minVel = -1.0, maxVel = 1.0;
    double w = 0.5, c1 = 1.5, c2 = 1.5;

    std::vector<Particle> particles(particleCount);
    for (auto& particle : particles) {
        initializeParticle(particle, dimensions, minPos, maxPos, minVel, maxVel);
    }

    std::vector<double> globalBestPosition = particles[0].bestPosition;
    double globalBestFitness = particles[0].bestFitness;

    for (const auto& particle : particles) {
        if (particle.bestFitness < globalBestFitness) {
            globalBestFitness = particle.bestFitness;
            globalBestPosition = particle.bestPosition;
        }
    }

    for (int iter = 0; iter < maxIter; ++iter) {
        for (auto& particle : particles) {
            updateParticle(particle, globalBestPosition, w, c1, c2);
        }
        for (const auto& particle : particles) {
            if (particle.bestFitness < globalBestFitness) {
                globalBestFitness = particle.bestFitness;
                globalBestPosition = particle.bestPosition;
            }
        }
        std::cout << "Iteration " << iter << ", Best Fitness: " << globalBestFitness << std::endl;
    }

    return 0;
}
