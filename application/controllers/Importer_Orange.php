<?php

    require 'vendor/autoload.php';

    use PhpOffice\PhpSpreadsheet\Spreadsheet;
    use PhpOffice\PhpSpreadsheet\Writer\Xlsx;
    // use PhpOffice\PhpSpreadsheet\Writer\Xls;
    // use PhpOffice\PhpSpreadsheet\Writer\Csv;

    class Importer_Orange extends CI_Controller {
       
        public function __construct(){
            parent::__construct();
            $this->load->model("Boa_Orange_Anomalie_Model");
            $this->load->model("Orange_Normal_Model");
            $this->load->model("Orange_Anomalie_Model");
            $this->load->model("Historique_Orange_Model");
            ini_set('memory_limit', '1024M');
            set_time_limit(300);
            session_start();
        }

        public function index() {

            $this->load->view("templates/sidebar");
            $this->load->view("pages/importer/importer");
            $this->load->view("pages/operateur/orange-form");
            
        }

        public function importer() {
            $orange_read = $this->readData($_FILES["orange"]["name"], $_FILES["orange"]["tmp_name"]);
            $princ_read = $this->readData($_FILES["princ"]["name"], $_FILES["princ"]["tmp_name"]);
            $com_read= $this->readData($_FILES["com"]["name"], $_FILES["com"]["tmp_name"]);

            if (count($orange_read) > 1 ) {
                
                for($i=1; $i < count($orange_read); $i++) {

                    $debit= str_replace([" ", ","], "", $orange_read[$i][6]);
                    $credit= str_replace([" ", ","], "", $orange_read[$i][7]);
                    $cle= $orange_read[$i][5] .$debit .$credit ;
                    $super_distri= str_replace([" ", ".0"], "", $orange_read[$i][8]); 
                    $sous_distr= str_replace([" ", ".0"], "", $orange_read[$i][9]);   
                    
                    $date = date_create_from_format('d/m/Y', $orange_read[$i][1]);
                    if ($date !== false) {
                        $date = $date->format('Y-m-d');
                    }
                    $orange[]= array(
                       "orange_num" => $orange_read[$i][0],
                       "orange_date" =>  $date,
                       "orange_heure" => $orange_read[$i][2],
                       "orange_ref" => $orange_read[$i][3],
                       "orange_service" => $orange_read[$i][4],
                       "orange_num_compte" => $orange_read[$i][5],
                       "orange_debit" => $debit,
                       "orange_credit" => $credit,
                       "orange_super_distri" => $super_distri,
                       "orange_sous_distri" => $sous_distr,
                       "cle" => $cle
                    );
                }
            }

            if (count($princ_read) > 1 ) {
                
                for($i=0; $i < count($princ_read); $i++) {

                    $montant = strval($princ_read[$i][4]);
                    $montant = str_replace(".00", "" , $montant);
                    $date_val = $princ_read[$i][2];
                    $date_val = date_create_from_format('d.m.Y', $date_val);
                
                    $date_oper = $princ_read[$i][1];
                    $date_oper = date_create_from_format('d.m.Y', $date_oper);

                    if ($date_oper !== false) {
                        $date_oper = $date_oper->format('Y-m-d');
                    }
                    if ($date_val !== false) {
                        $date_val = $date_val->format('Y-m-d');
                    }
                    $princ[]= array(
                       "princ_compte" => $princ_read[$i][0],
                       "princ_date_oper" =>  $date_oper,
                       "princ_date_val" => $date_val,
                       "princ_devise" => $princ_read[$i][3],
                       "princ_montant" => $montant,
                       "princ_libelle" => $princ_read[$i][5],
                       "princ_oper" => $princ_read[$i][6],
                       "princ_expl" => $princ_read[$i][7],
                       "princ_ref_igor" => $princ_read[$i][8],
                       "princ_ref_rel" => $princ_read[$i][10],
                    );
                }
            }
            if (count($com_read) > 1 ) {
                
                for($i=0; $i < count($com_read); $i++) {

                    $codeAgence= $com_read[$i][10];
                    $codeAgence= explode("-", $codeAgence);

                    $date_val= $com_read[$i][2];
                    $date_val = date_create_from_format('d.m.Y', $date_val);
                    
                    $date_oper= $com_read[$i][1];
                    $date_oper = date_create_from_format('d.m.Y', $date_oper);

                    if ($date_oper !== false) {
                        $date_oper = $date_oper->format('Y-m-d');
                    }
                    if ($date_val !== false) {
                        $date_val = $date_val->format('Y-m-d');
                    }
                    $comm[]= array(
                       "comm_compte" => $com_read[$i][0],
                       "comm_date_oper" =>  $date_oper,
                       "comm_date_val" => $date_val,
                       "comm_devise" => $com_read[$i][3],
                       "comm_montant" => $com_read[$i][4],
                       "comm_libelle" => $com_read[$i][5],
                       "comm_oper" => $com_read[$i][6],
                       "comm_expl" => $com_read[$i][7],
                       "comm_ref_igor" => $com_read[$i][8],
                       "comm_ref_rel" => $com_read[$i][10],
                       "comm_code_agence" => $codeAgence[0]
                    );
                }
            }



// --------------------------------------------------------------------------- PRINCIPALE -----------------------------------------------------------------

            $princ= $this->supprimerEspace($princ);
            $principalCI= $this->filtrerPrincCI($princ);
            $principalCO= $this->filtrerPrincCO($princ);
            $nonAuCI= $this->filtrerNonAuCI($princ);
            $nonAuCO= $this->filtrerNonAuCO($princ);
            $vi= $this->filtrerVi($princ);

            $principalCI= $this->trierPrincParRefIgor($principalCI);
            $principalCO= $this->trierParCleCroissante($principalCO);

            unset($princ);


// // --------------------------------------------------------------------------- COMMISSION -----------------------------------------------------------------
            $comm= $this->supprimerEspace($comm);
            $commCI= $this->filtrerCommCI($comm);
            $commCI= $this->trierCommParRefIgor($commCI);

            $mergedComEtPrinc = $this->mergePrincEtCommi($principalCI, $commCI);
            $mergedComEtPrinc = $this->trierParCleCroissante($mergedComEtPrinc);

            unset($comm);


// // ---------------------------------------------------------------------------- ORANGE -------------------------------------------------------------------------------
            $orange= $this->supprimerEspace($orange);
            $ind= $this->filtrerInd($orange);

            $orangeCI= $this->filtrerOrangeCI($orange);
            $orangeCI= $this->trierParCleCroissante($orangeCI);
            $orangeCO= $this->filtrerOrangeCO($orange);
            $orangeCO = $this->trierParCleCroissante($orangeCO);

            unset($orange);


// //  ---------------------------------------------------------------------------- COMAPARAISON -------------------------------------------------------------------------------

            $resultatOrangeCI= $this->comparerBoaEtOrange($mergedComEtPrinc, $orangeCI);
            $resultatOrangeCO= $this->comparerBoaEtOrange($principalCO, $orangeCO);


// // -----------------------------------------------------------------------------RESULTAT -------------------------------------------------------------------------------------

            $normalCI= $resultatOrangeCI[0];
            $normalCO= $resultatOrangeCO[0];
            $dat= $resultatOrangeCI[1];
            $cat= $resultatOrangeCO[1];
            $cleCI = $resultatOrangeCI[3];
            $cleCO = $resultatOrangeCO[3];
            $anomalieOrangeCI= $resultatOrangeCI[2];
            $anomalieOrangeCO= $resultatOrangeCO[2];


// ------------------------------------------------------------------------------- HISTORIQUE -----------------------------------------------------------

            $historique= array_merge($normalCI[0], $normalCO[0], $dat[0], $cat[0], $anomalieOrangeCI[0], $anomalieOrangeCO[0], $ind, $nonAuCO, $nonAuCI, $vi);
            $_SESSION["last_solde"] = $this->Historique_Orange_Model->get_last_solde();

            if(count($_SESSION["last_solde"]) > 0 ) {
                $princ_solde= $_SESSION["last_solde"][0]->princ_solde;
                $comm_solde= $_SESSION["last_solde"][0]->comm_solde;
                $orange_solde= $_SESSION["last_solde"][0]->orange_solde;
                $solde= $_SESSION["last_solde"][0]->solde;
            } else if(count($_SESSION["last_solde"]) === 0 ) {
                $princ_solde= 0;
                $comm_solde= 0;
                $orange_solde= 0;
                $solde= 0;
            }

            foreach($historique as $item ) {
                $princ_solde += isset($item["princ_montant"])  ? $item["princ_montant"] : 0;
                $comm_solde += isset($item["comm_montant"])  ? $item["comm_montant"] : 0;
                $orange_solde += isset($item["orange_montant"])  ? $item["orange_montant"] : 0;
                $solde += isset($item["orange_super_distri"])  ? $item["orange_super_distri"] : 0;

                $item["princ_solde"] = $princ_solde;
                $item["comm_solde"] = $comm_solde;
                $item["orange_solde"] = $orange_solde;
                $item["solde"] = $solde;

                $this->Historique_Orange_Model->insert($item);
            }

            



// // --------------------------------------------------------------------------------- REGULARISATION ------------------------------------------------------------------------------



// // ---------------------------------------------------------------------------------- INSERTION ----------------------------------------------------------------------------------
            
         
            foreach($normalCI[0] as $item) {
                $this->Orange_Normal_Model->insert_or_update_ci($item);
            }


            foreach($normalCO[0] as $item) {
                $this->Orange_Normal_Model->insert_or_update_co($item);
            }

            foreach ($dat[0] as $item ) {
                $this->Boa_Orange_Anomalie_Model->insert_or_update_dat($item);
            }


            foreach ($cat[0] as $item ) {
                $this->Boa_Orange_Anomalie_Model->insert_or_update_cat($item);
            }

            foreach($ind as $item) {
                $this->Orange_Anomalie_Model->insert_or_update_ind($item);
            }

            foreach($nonAuCI as $item) {
                $this->Boa_Orange_Anomalie_Model->insert_or_update_nonAuCI($item);
            }

            foreach($anomalieOrangeCI[0] as $item) {
                $this->Orange_Anomalie_Model->insert_or_update_ci($item);
            }

            foreach($anomalieOrangeCO[0] as $item) {
                $this->Orange_Anomalie_Model->insert_or_update_co($item);
            }
            foreach($vi as $item) {
                $this->Boa_Orange_Anomalie_Model->insert_or_update_vi($item);
            }
            
            $this->exporter($normalCI[0], $normalCO[0], $dat[0], $cat[0], $ind, $vi, $nonAuCI, $nonAuCO, $anomalieOrangeCI[0], $anomalieOrangeCO[0]);
            $_SESSION["importer"] ="OUI";
            redirect("importer_orange");
        }

        public function filtrerOrangeCI($data) {
            $resultat= array();
            foreach ($data as $item) {
                if(!empty($item["orange_debit"]) && empty($item["orange_credit"]) && $item["orange_num_compte"] !=="IND01") {
                    $item["orange_montant"]= $item["orange_debit"] * -1;
                    $resultat[]= $item;

                }
            }
            return $resultat;
        }

        public function filtrerOrangeCO($data) {
            $resultat= array();
            foreach ($data as $item) {
                if(empty($item["orange_debit"]) && !empty($item["orange_credit"]) && $item["orange_num_compte"] !=="IND01") {
                    $item["orange_montant"]= $item["orange_credit"] * 1;
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }


        public function readData($upload_file, $upload_tmp) {
            $extension = pathinfo($upload_file, PATHINFO_EXTENSION);

            if($extension ==="csv") {
                $reader= new \PhpOffice\PhpSpreadsheet\Reader\Csv();

            } else if ($extension =="xls") {
                $reader= new \PhpOffice\PhpSpreadsheet\Reader\Xls();
            } else {
                $reader=  new \PhpOffice\PhpSpreadsheet\Reader\Xlsx();
            }

            $spreadsheet= $reader->load($upload_tmp);
            $sheetdata= $spreadsheet->getActiveSheet()->toArray();

            return $sheetdata;

        }


        public function filtrerInd($data) {
            $resultat= array();

            foreach($data as $item) {
                if ($item["orange_num_compte"] ==="IND01") {
                    $item["orange_montant"] = $item["orange_debit"] * -1;
                    $resultat[] = $item;
                }
            }
            return $resultat;
        }

        public function filtrerVI($data) {
            $resultat= array();

            foreach($data as $item) {
                if (isset($item["princ_oper"]) && $item["princ_oper"] !=="CASHI" && $item["princ_oper"] !=="CASHO") {
                    $resultat[] = $item;
                }
                if (isset($item["comm_oper"]) && $item["comm_oper"] !=="CASHI") {
                    $resultat[] = $item;
                }
            }
            return $resultat;
        }

        public function supprimerEspace($data) {
            $resultat= array();
            foreach($data as $key=> $valeur) {
                if ($key !== 'LIBELLE') {
                    $resultat[$key] = str_replace(" ", "", $valeur);
                } else {
                    $resultat[$key] = $valeur;
                }
                // $resultat[$key] = str_replace(" ", "", $valeur);
            }

            return $resultat;
        }

        public function supprimerEspaceOrange($data) {
            foreach ($data as &$item) {
                foreach ($item as $key => &$value) {
                    $value = preg_replace('/\s+/', '', $value);
                }
                $item['orange_debit'] = preg_replace('/\s+/', '', $item['orange_debit']);
            }
            return $data;
        }


        public function filtrerPrincCI($data) {
            $resultat= array();

            foreach($data as $item) {
                if (preg_match('/tel:(\d+)/', $item["princ_libelle"], $matches)) {
                    $numeroTelephone = $matches[1];
                    // echo "Numéro de téléphone : " . $numeroTelephone;
                } else {
                    // echo "Aucun numéro de téléphone trouvé.";
                }
                if($item["princ_oper"] ==="CASHI" && $item["princ_expl"] ==="AU") {
                    $cle = $numeroTelephone .$item["princ_montant"];
                    $item["cle"] = $cle;
                    $resultat[]= $item;
                }
            }

            return $resultat;
        }

        public function filtrerPrincCO($data) {
            $resultat= array();

            foreach($data as $item) {
                if (preg_match('/tel:(\d+)/', $item["princ_libelle"], $matches)) {
                    $numeroTelephone = $matches[1];
                    // echo "Numéro de téléphone : " . $numeroTelephone;
                } else {
                    // echo "Aucun numéro de téléphone trouvé.";
                }
                if($item["princ_oper"] ==="CASHO" && $item["princ_expl"] ==="AU") {
                    $cle = $numeroTelephone .-$item["princ_montant"];
                    $item["cle"] = $cle;
                    $resultat[]= $item;
                }
            }

            return $resultat;
        }

        public function filtrerCommCI($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["comm_oper"] ==="CASHI" && $item["comm_expl"] ==="AU") {
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }

        public function filtrerNonAuCI($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["princ_oper"] ==="CASHI" && $item["princ_expl"] !=="AU") {
                    $resultat[]= $item;
                }
            }
            return $resultat;    
        }




        public function filtrerNonAuCO($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["princ_oper"] ==="CASHO" && $item["princ_expl"] !=="AU") {
                    $resultat[]= $item;
                }
            }
            return $resultat;
            
        }


        public function mergePrincEtCommi($principal, $commission) {
            $normalePrinc= array();
            $normaleComm= array();
            $mergedComEtPrinc= array();

            foreach($principal as $itemPrinc) {
                foreach($commission as $itemComm) {
                    if($itemPrinc["princ_ref_igor"] === $itemComm["comm_ref_igor"]) {
                        $normalePrinc[] = $itemPrinc;
                    }
                }
            }
            foreach($commission as $itemComm) {
                foreach($principal as $itemPrinc) {
                    if($itemPrinc["princ_ref_igor"] === $itemComm["comm_ref_igor"]) {
                        $normaleComm[] = $itemComm;
                    }
                }
            }

            $ligne= count($normalePrinc) > count($normaleComm) ? $normalePrinc : $normaleComm;

            for ($i =0; $i < count($normalePrinc) ; $i++) {
                $mergedComEtPrinc[] = array_merge($normalePrinc[$i], $normaleComm[$i]);
            }

            return $mergedComEtPrinc;
        }

        public function trierCommParRefIgor($data) {
            usort($data, function($a, $b) {
                return strcmp($a['comm_ref_igor'], $b['comm_ref_igor']);
            });

            return $data;
        }

        public function trierPrincParRefIgor($data) {
            usort($data, function($a, $b) {
                return strcmp($a['princ_ref_igor'], $b['princ_ref_igor']);
            });

            return $data;
        }

        public function trierParCleCroissante($data){
            usort($data, function ($a, $b) {
                $cleA = intval($a['cle']);
                $cleB = intval($b['cle']);

                if ($cleA == $cleB) {
                    return 0;

                }
                return ($cleA < $cleB) ? -1 : 1;
            });

            return $data;
        }


        public function comparerBoaEtOrange ($igorTab, $orangeTab) {
            $orangeNormal = array();
            $boaNormal = array();
            $boaAnomalie = array();
            $orangeAnomalie = array();
            $cle= array();

            $orangeCount = count($orangeTab);
            $boaCount = count($igorTab);

            $i = 0;
            $j = 0;

            while ($i < $boaCount && $j < $orangeCount) {
                $cleIgor = $igorTab[$i]["cle"];
                $cleOrange = $orangeTab[$j]["cle"];
                $difference = $cleIgor - $cleOrange;
                if ($difference === 0) {
                    $orangeNormal[] = $orangeTab[$j];
                    $boaNormal[] = $igorTab[$i];
                    $i++;
                    $j++;
                } elseif ($difference < 0) {
                    $boaAnomalie[] = $igorTab[$i];
                    $cle[]= $cleIgor;
                    $i++;
                    
                } else {
                    $orangeAnomalie[] = $orangeTab[$j];
                    $j++;
                }
            }

            // Traiter les éléments restants dans $igorTab comme des anomalies
            while ($i < $boaCount) {
                $boaAnomalie[] = $igorTab[$i];
                $i++;
            }

            // Traiter les éléments restants dans $orangeTab comme des anomalies
            while ($j < $orangeCount) {
                $orangeAnomalie[] = $orangeTab[$j];
                $j++;
            }


            for($i=0; $i < count($boaNormal); $i++) {
                $mergeBoaOrange[]= array_merge($boaNormal[$i], $orangeNormal[$i]);
            } 

            // echo "<pre>";
            //     print_r($cle);
            // echo "</pre>";
            return [[$mergeBoaOrange], [$boaAnomalie], [$orangeAnomalie], [$cle]];
        }

        public function rechercheCle($cle, $boa, $orange) {
                $boaResultat= array();
                $orangeResultat = array();
                foreach($cle as $itemCle) {
                    foreach($boa as $item) {
                        if($item["cle"] === $itemCle) {
                            $boaResultat[]= $item;
                        }
                    }
                }
                foreach($cle as $itemCle) {
                    foreach($orange as $item) {
                        if($item["cle"] === $itemCle) {
                            $orangeResultat[]= $item;
                        }
                    }
                }

                // echo "<pre>" . "orange";
                //     print_r($boaResultat);
                // echo "</pre>";

                // $occurrences = array();
                // $resultats = array();
                // $nbreOccurenceUn= array();

                // foreach ($boaResultat as $element) {
                //     $cle = $element['cle'];
                //     if (isset($occurrences[$cle])) {
                //         $occurrences[$cle]++;
                //     } else {
                //         $occurrences[$cle] = 1;
                //     }
                // }
            
                // foreach ($boaResultat as $index => $element) {
                //     $cle = $element['cle'];
                //     if ($occurrences[$cle] === 1) {
                //         $nbreOccurenceUn[]= $boaResultat[$index];
                //         unset($boaResultat[$index]);
                //     } else {
                //         $resultats[] = $element;
                //     }
                // }
            
                // $boaResultat = array_values($boaResultat);

                // echo "<pre>" . "boa";
                //     print_r($boaResultat);
                // echo "</pre>";
                // echo "<pre>" . "orange";
                //     print_r($orangeResultat);
                // echo "</pre>";
        }

        public function exporter($normalCI, $normalCO, $dat, $cat, $ind, $vi, $nonAuCI, $nonAuCO, $anomalieOrangeCI, $anomalieOrangeCO) {
            $dateAujourdhui = date("Y-m-d");
            $nomFichier = "RapproOrange-" .$dateAujourdhui .".xlsx";

            header("Content-Type: application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
            header("Content-Disposition: attachment; filename=\"" . $nomFichier . "\"");
            
            $spreadsheet = new PhpOffice\PhpSpreadsheet\Spreadsheet();
            $sheet = $spreadsheet->getActiveSheet();

            $cell_array = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "AA", "AB", "AC", "AD", "AE", "AF", "AG", "AH", "AI", "AJ", "AK", "AL", "AM"];
            $header = ["DATE OPE", "DATE VAL", "DEVISE", "MONTANT", "LIBELLE", "CODE OPER", "EXPL", "REF IGOR", "REF_REL", "PHONE","SOLDE", "", "DATE_OPER", "DATE_VAL", "DEVISE", "MONTANT", "LIBELLE", "CODE", "EXPL", "REF_IGOR", "OPER_ID", "CODE AGENCE","SOLDE", "", "DATE", "HEURE", "REFERENCE", "SERVICE", "NUM_COMPTE", "DEBIT", "CREDIT", "MONTANT", "SOLDE", "DEBIT", "CREDIT", "MONTANT", "SOLDE", "PRINCIPALE", "COMM", "PRINCIPAL", "COMM"];



            $sheet->mergeCells('A1:W1');
            $sheet->getStyle('A1:W1')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->setCellValue('A1', 'BOA');
            $sheet->getStyle('A1')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('A1')->getFont()->setBold(true);

            $sheet->mergeCells('A2:K2');
            $sheet->getStyle('A2:J2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->setCellValue('A2', 'PRINCIPALE');
            $sheet->getStyle('A2')->getFont()->setBold(true);
            $sheet->getStyle('A2')->getAlignment()->setHorizontal('center');
            
            $sheet->mergeCells('L2:W2');
            $sheet->getStyle('L2:W2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->setCellValue('L2', 'COMMISSION');
            $sheet->getStyle('L2')->getFont()->setBold(true);
            $sheet->getStyle('L2')->getAlignment()->setHorizontal('center');

            $sheet->mergeCells('Y1:AK2');
            $sheet->getStyle('Y1:AK2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->setCellValue('Y1', 'ORANGE');
            $sheet->getStyle('Y1')->getFont()->setBold(true);
            $sheet->getStyle('Y1')->getAlignment()->setHorizontal('center');

            $sheet->getStyle('A2')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('M2:W2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->getStyle('M2:AH2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->getStyle('A1')->getFont()->setBold(true);
            $sheet->getStyle('M1')->getFont()->setBold(true);
            $sheet->getStyle('A2')->getFont()->setBold(true);
            $sheet->getStyle('M2')->getFont()->setBold(true);

            
            //  BORDURE :
            for($i=0; $i < count($cell_array) -3 ; $i++) {
                $sheet->getStyle($cell_array[$i] . "3")->getBorders()->getAllBorders()->setBorderStyle('thin');
            }

            // EN TETE 

            for($i=0; $i < count($cell_array) ; $i++) {
                $sheet->getStyle($cell_array[$i] . "3")->getBorders()->getAllBorders()->setBorderStyle('thin');
                $sheet->setCellValue($cell_array[$i] ."3", $header[$i]);
                $sheet->getStyle($cell_array[$i] . "3")->getAlignment()->setHorizontal('center');
                $sheet->getStyle($cell_array[$i] . "3")->getFont()->setBold(true);
                $sheet->getColumnDimension($cell_array[$i])->setAutoSize(true);
            }

            $lastRow = $sheet->getHighestRow() + 2;
            foreach ($normalCI as $dataRow) {
                $i=0;
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_date_oper"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_date_val"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_devise"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_montant"]); 
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_libelle"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_oper"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_expl"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_ref_igor"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_ref_rel"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, substr($dataRow["cle"], 0, 10));
                $i+=2;
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["comm_date_oper"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_date_val"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_devise"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_montant"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_libelle"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_oper"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_expl"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_ref_igor"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_ref_rel"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_code_agence"]);
                $i+=2;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_date"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_heure"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_ref"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_service"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_num_compte"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_debit"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_credit"]);
                // $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_debit"] * -1);

                $i+=1;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_sous_distri"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_super_distri"]);
                $lastRow++;
            }

            $lastRow = $sheet->getHighestRow() + 1;
            foreach ($normalCO as $dataRow) {
                $i=0;
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_date_oper"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_date_val"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_devise"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_montant"]); 
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_libelle"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_oper"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_expl"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_ref_igor"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_ref_rel"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, substr($dataRow["cle"], 0, 10));
                $i+=14;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_date"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_heure"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_ref"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_service"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_num_compte"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_debit"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_credit"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_credit"] * 1 );
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_sous_distri"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_super_distri"]);
                $lastRow++;
            }


            $lastRow = $sheet->getHighestRow() + 2;
            foreach ($dat as $dataRow) {
                $i=0;

                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                
                $i=0;

                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_date_oper"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_date_val"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_devise"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_montant"]); 
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_libelle"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_oper"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_expl"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_ref_igor"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_ref_rel"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, substr($dataRow["cle"], 0, 10));
                $i+=2;
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["comm_date_oper"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_date_val"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_devise"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_montant"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_libelle"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_oper"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_expl"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_ref_igor"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_ref_rel"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["comm_code_agence"]);
                $lastRow++;
            }
            
            $lastRow = $sheet->getHighestRow() + 2;

            foreach ($cat as $dataRow) {
                $i=0;

                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $i=0;
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_date_oper"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_date_val"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_devise"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_montant"]); 
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_libelle"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_oper"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_expl"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_ref_igor"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_ref_rel"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, substr($dataRow["cle"], 0, 10));
                $lastRow++;
            }

            $lastRow = $sheet->getHighestRow() + 2;
            foreach ($vi as $dataRow) {
                $i=0;
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('66CCFF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('66CCFF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('66CCFF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('66CCFF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('66CCFF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('66CCFF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('66CCFF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('66CCFF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('66CCFF');
                $i=0;
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_date_oper"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_date_val"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_devise"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_montant"]); 
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_libelle"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_oper"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_expl"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_ref_igor"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["princ_ref_rel"]);
                $lastRow++;
            }

            $lastRow = $sheet->getHighestRow();
            foreach($ind as $dataRow) {
                $i=24;
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                
                $i=24;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_date"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_heure"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_ref"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_service"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_num_compte"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_debit"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_credit"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_sous_distri"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_super_distri"]);
                $lastRow++;
            }

            $lastRow = $sheet->getHighestRow() + 2;
            foreach($anomalieOrangeCI as $dataRow) {
                $i=24;
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');

                $i=24;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_date"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_heure"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_ref"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_service"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_num_compte"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_debit"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_credit"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_sous_distri"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_super_distri"]);
                // $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["cle"]);
                $lastRow++;
            }
            $lastRow = $sheet->getHighestRow() + 2;
            foreach($anomalieOrangeCO as $dataRow) {
                $i=24;
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('AAFF00');

                $i=24;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_date"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_heure"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_ref"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_service"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_num_compte"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_debit"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_credit"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_sous_distri"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["orange_super_distri"]);
                // $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["cle"]);
                $lastRow++;
            }

            $lastRow = $sheet->getHighestRow() + 1;
            $range= 'K4:K' .$lastRow;
            
            for ($row = 5; $row <= $lastRow; $row++) {
                $formula = "=D$row + K" . ($row - 1);
                $sheet->setCellValue("K$row", $formula);
            }

            for ($row = 5; $row <= $lastRow; $row++) {
                $formula = "=P$row + W" . ($row - 1);
                $sheet->setCellValue("W$row", $formula);
            }
            for ($row = 5; $row <= $lastRow; $row++) {
                $formula = "=AF$row + AG" . ($row - 1);
                $sheet->setCellValue("AG$row", $formula);
            }
            for ($row = 5; $row <= $lastRow; $row++) {
                $formula = "=AF$row + AG" . ($row - 1);
                $sheet->setCellValue("AG$row", $formula);
            }
            for ($row = 5; $row <= $lastRow; $row++) {
                $formula = "=AH$row + AI$row";
                $sheet->setCellValue("AJ$row", $formula);
            }
            for ($row = 5; $row <= $lastRow; $row++) {
                $formula = "=AJ$row + AK" .($row - 1);
                $sheet->setCellValue("AK$row", $formula);
            }

            for ($row = 5; $row <= $lastRow; $row++) {
                $formula = "=D$row + AF$row" ;
                $sheet->setCellValue("AL$row", $formula);
            }
            for ($row = 5; $row <= $lastRow; $row++) {
                $formula = "=P$row + AJ$row" ;
                $sheet->setCellValue("AM$row", $formula);
            }

            $writer = new PhpOffice\PhpSpreadsheet\Writer\Xlsx($spreadsheet);
            $writer->save('php://output');

    }
}

?>