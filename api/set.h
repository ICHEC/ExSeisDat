struct ExSeisSetWrapper
{
    PIOL::File::Direct * file;
};
typedef struct ExSeisSetWrapper ExSeisSet;

ExSeisSet makeSet(ExSeisHandle piol, const char * ptrn)
{
    auto wrap = new SetWrapper;
    wrap->set = std::make_shared<Set>(*piol->piol, ptrn);
    return wrap;
}

void dropSet(ExSeisSet s)
{
    if (s != NULL)
    {
        if (s->set != NULL)
            delete s->set;
        delete s;
    }
    else
        std::cerr << "Invalid free of ExSeisSet NULL.\n";
}

void getMinMaxSet(ExSeisSet s, Meta m1, Meta m2, CoordElem * minmax)
{
    s->set->getMinMax(m1, m2, minmax);
}

void sortSet(ExSeisSet s, bool (* func)(const Param *, const Param *))
{
    s->set->sort([func] (const Param & a, const Param & b) -> bool { return func(&a, &b); });
}

void defsortSet(ExSeisSet s, SortType type)
{
    s->set->sort(type);
}

size_t getInNt(ExSeisSet s)
{
    return s->set->getInNt();
}

size_t getLNtSet(ExSeisSet s)
{
    return s->set->getLNt();
}

void outputSet(ExSeisSet s, const char * oname)
{
    return s->set->output(oname);
}

void textSet(ExSeisSet s, const char * outmsg)
{
    s->set->text(outmsg);
}

void summarySet(ExSeisSet s) const
{
    s->set->summary();
}

void addSet(ExSeisSet s, const char * name)
{

}


