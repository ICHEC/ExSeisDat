




A(il, xl, i) =  R(il, xl, (V(il, xl, CLOSEST(i)) / (cos(\Theta(i)) * D(il, xl, CLOSEST(i))) ) / N_r)

void convRadon2Angle(File::Interface * vm)
{
    size_t aNs; //Input
    size_t gOffset;
    size_t agSz;
    size_t vNs = vm->readNs();

    std::vector<size_t> vtrc(numGather * vNs);
    vm.readTrace(gOffset, numGather, vtrc.data());

    size_t rNs = radon->readNs();

    geom_t vinc = vm->readInc();
    geom_t rinc = radon->readInc();

    geom_t ainc = pi / geom_t(180.0);   //1 degree
    size_t lOffset = gOffset;

    for (size_t i = 0; i < numGather; i++)
    {
        const size_t rgSz = std::get<0>(gather[i]);

        std::vector<trace_t> rtrc(rgSz * rNs);
        std::vector<trace_t> atrc(agSz * aNs);

        radon->readTrace(loffset, rgSz, rtrc.data());

        for (size_t j = 0; j < agSz; j++)       //For each angle in the angle gather
            for (size_t z = 0; z < rNs; z++)    //For each sample (angle + radon)
            {
                geom_t vmModel = vtrc[i * vNs + size_t(std::min(geom_t(z) * rinc) / vinc, vNs)];
                llint k = llround(vmModel / (cos(j * ainc) * geom_t(std::get<0>(gather[i]))));
                if (k > 0 && k < std::get<0>(gather[i]))
                    atrc[j * agSz + z] = rtrc[k * rgSz + z];
            }

        auto rule = std::make_shared<File::Rule>(true, true);

        File::Param aprm(rule, agSz);
        for (size_t j = 0; j < agSz; j++)
        {
            //TODO: Set the rest of the parameters
            //TODO: Check the get numbers
            File::setPrm(j, Meta::il, std::get<1>(gather[i]), &aprm);
            File::setPrm(j, Meta::xl, std::get<2>(gather[i]), &aprm);
        }

        angle->writeTrace(loffset, agSz, atrc.data(), &aprm);

        loffset += rgSz;
    }

    for (size_t j = 0; j < extra; j++)
    {
        radon->readTrace(0U, 0U, (trace_t *)(nullptr));
        angle->writeTrace(0U, 0U, (trace_t *)(nullptr), (File::Param *)(nullptr));
    }
}

