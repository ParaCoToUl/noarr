#ifndef NOARR_PIPELINES_DEBUGGING_SCHEDULER_HPP
#define NOARR_PIPELINES_DEBUGGING_SCHEDULER_HPP

#include <iostream>
#include <string>
#include <cassert>
#include <vector>

#include "Node.hpp"
#include "SchedulerLogger.hpp"

namespace noarr {
namespace pipelines {

/*
    TODO: implement a proper "Scheduler" class that will do parallelism

    A naive scheduler implementation with no parallelism.

    TODO: parallelize memory transfers, device computation
        and host computation
 */

/**
 * A scheduler that executes nodes in a deterministic, synchronous way
 */
class DebuggingScheduler {
private:
    std::unique_ptr<SchedulerLogger> logger;

    /**
     * Nodes that the scheduler periodically updates
     */
    std::vector<Node*> nodes;

public:

    DebuggingScheduler() {
        //
    }

    DebuggingScheduler(std::ostream& output_stream) {
        this->logger = std::make_unique<SchedulerLogger>(output_stream);
    }

    ///////////////////////////////
    // Pipeline construction API //
    ///////////////////////////////

    /**
     * Registers a node to be updated by the scheduler
     */
    void add(Node& node) {
        this->nodes.push_back(&node);

        if (logger)
            logger->after_node_added(node);
    }

    ///////////////////
    // Execution API //
    ///////////////////

    /**
     * Runs the pipeline until no nodes can be advanced.
     */
    void run() {
        this->initialize_pipeline(); // NOTE: need not be called explicitly

        while (!this->pipeline_terminated) {
            this->update_next_node();
            
            // NOTE: termination is performed automatically
        }
    }

    /////////////////////////////////////////////////////
    // Lower level API for more fine-grained debugging //
    /////////////////////////////////////////////////////

private:
    bool pipeline_initialized = false;
    bool pipeline_terminated = false;

    bool generation_advanced_data = false; // did this generation advance any data?
    std::size_t current_generation = 0;
    std::size_t next_node = 0;

public:
    /**
     * Scheduler looks at the next node and updates it
     * (it goes over the nodes in the order they were inserted,
     * generation by generation)
     * @return True if data was advanced
     */
    bool update_next_node() {
        assert(this->nodes.size() != 0
            && "Pipeline is empty so cannot be advanced");

        assert(!this->pipeline_terminated
            && "Pipeline is already terminated");

        // pipeline initialization
        if (!this->pipeline_initialized)
            this->initialize_pipeline();
        
        // === update the node ===
        
        Node* node = this->nodes[this->next_node];

        if (this->logger)
            this->logger->before_node_updated(*node);

        // update (synchronous)
        bool data_was_advanced;
        this->callback_will_be_called();
        node->scheduler_update([this, &data_was_advanced](bool adv){
            data_was_advanced = adv;
            callback_was_called();
        });
        this->wait_for_callback(); // here's the synchronicity

        if (data_was_advanced) {
            this->generation_advanced_data = true;

            if (this->logger)
                this->logger->say("Node did advance data.");
        }

        // post update
        node->scheduler_post_update(data_was_advanced);

        // === move to the next node ===
        
        this->next_node++;

        // === move to the next generation ===

        if (this->next_node >= this->nodes.size()) {
            if (this->logger) {
                this->logger->say(
                    "Generation " + std::to_string(this->current_generation)
                    + " has ended."
                );
            }

            // pipeline termination
            if (!this->generation_advanced_data) {
                if (this->logger)
                    this->logger->say("Termination condition met.");

                this->terminate_pipeline();
            }
            
            this->next_node = 0;
            this->current_generation++;
            this->generation_advanced_data = false;
        }

        return data_was_advanced;
    }

private:
    /**
     * Called automatically before the first node is updated
     */
    void initialize_pipeline() {
        assert(!this->pipeline_initialized
            && "Pipeline was already initialized");

        if (this->logger)
            this->logger->say("Starting pipeline initialization...");

        for (Node* node : this->nodes) {
            if (this->logger)
                this->logger->say("Initializing node " + node->label + " ...");

            node->scheduler_initialize();
        }

        this->pipeline_initialized = true;

        if (this->logger)
            this->logger->say("Pipeline initialized.");
    }

    /**
     * Called automatically after the pipeline detects the stopping condition
     */
    void terminate_pipeline() {
        assert(!this->pipeline_terminated
            && "Pipeline was already terminated");

        if (this->logger)
            this->logger->say("Starting pipeline termination...");

        for (Node* node : this->nodes) {
            if (this->logger)
                this->logger->say("Terminating node " + node->label + " ...");
            
            node->scheduler_terminate();
        }

        this->pipeline_terminated = true;

        if (this->logger)
            this->logger->say("Pipeline terminated.");
    }

    ///////////////////////////
    // Synchronization logic //
    ///////////////////////////

    // TODO: Add a proper synchronization primitive on which the scheduler
    // thread can wait and let the callback_was_called method (or its
    // quivalent) be callable from any thread.

private:
    bool _expecting_callback = false;
    bool _callback_was_called = false;

    /**
     * Call this before initializeing a node update
     */
    void callback_will_be_called() {
        assert(!this->_expecting_callback
            && "Cannot expect a callback when the previous didn't finish.");

        this->_expecting_callback = true;
        this->_callback_was_called = false;
    }

    /**
     * Call this from the node callback
     */
    void callback_was_called() {
        this->_callback_was_called = true;
    }

    /**
     * Call this to synchronously wait for the callback
     */
    void wait_for_callback() {
        assert(this->_expecting_callback
            && "Cannot wait for callback without first expecting it.");

        // TODO: perform the actual wait here
        assert(this->_callback_was_called
            && "TODO: Asynchronous nodes are not implemented yet.");

        this->_expecting_callback = false;
    }

};

} // pipelines namespace
} // namespace noarr

#endif
