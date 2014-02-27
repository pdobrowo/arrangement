/**
 * Copyright (C) 2009-2013  Przemysław Dobrowolski
 *
 * This file is part of the Configuration Space Library (libcs), a library
 * for creating configuration spaces of various motion planning problems.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "tritripredicatelist.h"

std::vector<VariantPredicate> triTriPredicateList(
        const Vector_3_Z &a, const Vector_3_Z &b, const Vector_3_Z &c,
        const Vector_3_Z &k, const Vector_3_Z &l, const Vector_3_Z &m,
        bool optimize)
{
    if (optimize)
    {
        // radius check
        //RT maxRadius = std::max(a.length())
    }

    VariantPredicate predicates[4];

    predicates[0].type = VariantPredicate::Screw;
    predicates[0].s3 = Predicate_S_3_Z(k, l, a, b);
    predicates[1].type = VariantPredicate::Screw;
    predicates[1].s3 = Predicate_S_3_Z(k, l, c, b);
    predicates[2].type = VariantPredicate::Screw;
    predicates[2].s3 = Predicate_S_3_Z(m, l, a, b);
    predicates[3].type = VariantPredicate::Screw;
    predicates[3].s3 = Predicate_S_3_Z(m, l, c, b);

    return std::vector<VariantPredicate>(predicates, predicates + 4);
}
