#!/bin/bash

# use sbatch to lauch script

BASE_DIR=/scratch/VR0052/ESA2014/surf/experiments
#SBATCH -p turpin
#SBATCH --job-name=ex_wtd
#SBATCH --account="VR0280"
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --time=10-12:00:00
#SBATCH --mem=512GB
#SBATCH --mail-user simon.gog@unimelb.edu.au
#SBATCH --mail-type=BEGIN
#SBATCH --mail-type=END
#SBATCH --mail-type=FAIL

module load gcc
module load cmake

$BASE_DIR/mem_used.sh

## --exclusive
