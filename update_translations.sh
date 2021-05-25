#!/bin/bash

for file in translations/znotes_*.ts;
do lupdate *.cpp *.ui -ts $file;
done

