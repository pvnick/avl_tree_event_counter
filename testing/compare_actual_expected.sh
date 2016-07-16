#!/bin/bash 

find *\ test*.txt | xargs -i{} sh -c 'echo {}; pr -m -t  "expected_output/{}" "actual_output/{}"'
