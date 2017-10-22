# Rubix

## Author Information

Selir

## Description:
This service implements a simple 3x3 Rubix cube game.  The service prompts for 54 bytes of data (6-sided cube * 3 * 3) and stores that in a buffer which represents the cells of the cube.  It then scrambles the cube and asks the team to solve the cube back to its original state.  If successful, the service calls the buffer.

The poller for this service will excercise the cube rotation actions as well as the call to the buffer.  So, it will be impossible for the teams to simply patch out the call or break other team's ability to solve the cube.  That means teams will be forced to patch the service in a way that intelligently looks for attacks that are trying to retrieve the flag.

## Vulnerability

## Challenges
