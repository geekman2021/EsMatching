-- Copyright (C) 2007 MySQL AB
-- Use is subject to license terms
-- 
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; version 2 of the License.
-- 
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
-- 
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1335  USA

INSERT INTO time_zone_name (Name, Time_Zone_id) VALUES   ('MET', 1), ('UTC', 2), ('Universal', 2),    ('Europe/Moscow',3), ('leap/Europe/Moscow',4),    ('Japan', 5);
INSERT INTO time_zone (Time_zone_id, Use_leap_seconds)   VALUES (1,'N'), (2,'N'), (3,'N'), (4,'Y'), (5,'N');
INSERT INTO time_zone_transition   (Time_zone_id, Transition_time, Transition_type_id) VALUES   (1, -1693706400, 0) ,(1, -1680483600, 1)  ,(1, -1663455600, 2) ,(1, -1650150000, 3)  ,(1, -1632006000, 2) ,(1, -1618700400, 3)  ,(1, -938905200, 2) ,(1, -857257200, 3)  ,(1, -844556400, 2) ,(1, -828226800, 3)  ,(1, -812502000, 2) ,(1, -796777200, 3)  ,(1, 228877200, 2) ,(1, 243997200, 3)  ,(1, 260326800, 2) ,(1, 276051600, 3)  ,(1, 291776400, 2) ,(1, 307501200, 3)  ,(1, 323830800, 2) ,(1, 338950800, 3)  ,(1, 354675600, 2) ,(1, 370400400, 3)  ,(1, 386125200, 2) ,(1, 401850000, 3)  ,(1, 417574800, 2) ,(1, 433299600, 3)  ,(1, 449024400, 2) ,(1, 465354000, 3)  ,(1, 481078800, 2) ,(1, 496803600, 3)  ,(1, 512528400, 2) ,(1, 528253200, 3)  ,(1, 543978000, 2) ,(1, 559702800, 3)  ,(1, 575427600, 2) ,(1, 591152400, 3)  ,(1, 606877200, 2) ,(1, 622602000, 3)  ,(1, 638326800, 2) ,(1, 654656400, 3)  ,(1, 670381200, 2) ,(1, 686106000, 3)  ,(1, 7