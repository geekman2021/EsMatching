<?php

    require 'vendor/autoload.php';

    use PhpOffice\PhpSpreadsheet\Spreadsheet;
    use PhpOffice\PhpSpreadsheet\Writer\Xlsx;

    class Importer_Telma extends CI_Controller {

        public function __construct() {
            parent::__construct();
            $this->load->model("Telma_Normal_Model");
            $this->load->model("Telma_Anomalie_Model");
            $this->load->model("Boa_Telma_Anomalie_Model");
            $this->load->model("Historique_Telma_Model");
            ini_set('memory_limit', '1024M');
            set_time_limit(300);
            session_start();
        }

        public function index () {

            $this->load->view("templates/sidebar");
            $this->load->view("pages/importer/importer");
            $this->load->view("pages/operateur/telma-form");
            $_SESSION["dat"] = $this->Boa_Telma_Anomalie_Model->get_ci();
            $_SESSION["vi"]= $this->Boa_Telma_Anomalie_Model->get_vi();
            $_SESSION["admin"]= $this->Telma_Anomalie_Model->get_admin();

        }

        public function importer() {
        
            if ($_FILES && $_FILES['telma']['name'] && $_FILES["igor"]["name"]) {
                $file_path = $_FILES['telma']['tmp_name'];
                $telma= array();
                $igor_read= $this->readData($_FILES["igor"]["name"], $_FILES["igor"]["tmp_name"]);
                
                if (($handle = fopen($file_path, 'r')) !== FALSE) {
                    while (($row = fgetcsv($handle, 1000, ',')) !== FALSE) {
                        if (count($row) == 20) {
                            $date_str = substr($row[0], 0, 10);
                            $date = date_create_from_format('d/m/Y', $date_str);
                            if ($date !== false) {
                                $nouveau_format = $date->format('Y-m-d');
                            }
                            $telma[] = array(
                                'date_d' => $row[0],
                                'date' => $nouveau_format,
                                'trans_id' => $row[1],
                                'initiator' => $row[2],
                                'TYPE' => $row[3],
                                'Channel' => $row[4],
                                'State' => $row[5],
                                'Wallet' => $row[6],
                                'Amount_MGA' => str_replace(',', '', $row[7]), // Supprimez les virgules
                                'Sender' => $row[8],
                                'Sender_name' => $row[9],
                                'receiver' => $row[10],
                                'receiver_name' => $row[11],
                                'details1' => $row[12],
                                'Confirming_agent' => $row[13],
                                'notify_alternate' => $row[14],
                                'origine' => $row[15],
                                'TVA' => $row[16],
                                'ACTION' => $row[17],
                                'AA1_GROUP' => $row[18],
                                'PAR' => $row[19]
                            );
                            
                            // Insérez les données dans la base de données
                            // $this->Transaction_model->insert_transaction($data);
                        }
                    }
                    fclose($handle);

                    if (count($igor_read) > 1 ) {
                
                        for($i=0; $i < count($igor_read); $i++) {
                            $montant= str_replace(".00", "", $igor_read[$i][4]);
                            $date_val= $igor_read[$i][2];
                            $date_val = date_create_from_format('d.m.Y', $date_val);
                            
                            $date_oper= $igor_read[$i][1];
                            $date_oper = date_create_from_format('d.m.Y', $date_oper);

                            if ($date_oper !== false) {
                                $date_oper = $date_oper->format('Y-m-d');
                            }
                            if ($date_val !== false) {
                                $date_val = $date_val->format('Y-m-d');
                            }


                            $igor[]= array(
                               "COMPTE" => $igor_read[$i][0],
                               "DATE_OPER" =>  $date_oper,
                               "DATE_VAL" => $date_val,
                               "DEVISE" => $igor_read[$i][3],
                               "MONTANT" => $montant,
                               "LIBELLE" => $igor_read[$i][5],
                               "OPER" => $igor_read[$i][6],
                               "EXPL" => $igor_read[$i][7],
                               "REF_IGOR" => $igor_read[$i][8],
                               "ref_rel" => $igor_read[$i][9]
                            );
                        }
                    }


// -------------------------------------------------------------------------TELMA -----------------------------------------------------------------------------

                    $telma = $this->supprimerEspace($telma);
                    $telma = $this->trierParTransId($telma);

                    $reverseEtCle= $this->trouverIndexEtCleReverse($telma);

                    $indexAnnul= $this->trouverAnnulation($telma, $reverseEtCle);
                    $indexAdmin= $this->trouverIndexAdmin($telma);
                    $indexRollback = $this->trouverIndexRollBack($telma);

                    $indexSupprimer= array_merge($indexAnnul, $indexAdmin);

                    $indexSupprimer= array_merge($indexSupprimer, $indexRollback);

                    $reste= $this->supprimerIndices($telma, $indexSupprimer);
                    $admin = $this->filtrerAdmin($telma);
                    $reverse= $this->filtrerReverse($telma);

                    $rollback= $this->filtrerRollBack($telma);

                    $telmaCI= $this->filtrerCI($reste);
                    $telmaCI= $this->trierParCleEtHeure($telmaCI);
                    $telmaCI= $this->trierParCleEtHeure($telmaCI);

                    $telmaCO= $this->filtrerCO($reste);
                    $telmaCO= $this->trierParCleEtHeure($telmaCO);

                   


// -------------------------------------------------------------------------------------IGOR ---------------------------------------------------------------------------


                    $igor = $this->supprimerEspace($igor);
                    $vi= $this->filtrerVI($igor);
                    $nonAu= $this->filtrerNonAu($igor);
                    $igorCI= $this->filtrerCIIgor($igor);

                    
                    $igorCI= $this->trierParCleEtHeure($igorCI);
                    $igorCO= $this->filtrerCOIgor($igor);
                    $igorCO= $this->trierParCleEtHeure($igorCO);


                    $reverseEtAnnule= $this->reverseEtAnnule($indexAnnul, $telma);

// -----------------------------------------------------------------------------------COMPARAISON -------------------------------------------------------------------------
                    $resultatCI= $this->comparerTelmaEtIgor($igorCI, $telmaCI);
                    $resultatCO= $this->comparerTelmaEtIgor($igorCO, $telmaCO);
                    $normalCI= $resultatCI[0];
                    $normalCO= $resultatCO[0];
                    $dat= $resultatCI[1];
                    $cat= $resultatCO[1];
                    $mvtsCI= $resultatCI[2];
                    $mvtsCO= $resultatCO[2];
                    
                    $telmaAnomalieCI= $resultatCI[2];
                    $telmaAnomalieCO= $resultatCO[2];
                    $this->comparerTelmaEtIgor($igorCI, $telmaCI);

// ------------------------------------------------------------------------------REGULARISATION ----------------------------------------------------------------------

if(empty($_SESSION["admin"]) && empty($_SESSION["vi"]) && empty($admin) && !empty($vi)) {

    foreach($vi as $item) {
        $this->Boa_Telma_Anomalie_Model->insert_or_update_vi($item);
    }
} else if(empty($_SESSION["admin"]) && empty($_SESSION["vi"]) && !empty($admin) && empty($vi)) {

    foreach($admin as $item) {
        $this->Telma_Anomalie_Model->insert_or_update_admin($item);
    }
} else if (!empty($_SESSION["admin"]) && empty($_SESSION["vi"]) && !empty($admin) && !empty($vi)) {

    // $this->RegulAdminVi($vi, $admin);
    $adjust= array();
    foreach ($_SESSION["admin"] as $item) {
        $adjust[]= get_object_vars($item);
    }
    $jiaby= array_merge($adjust, $admin);
    $this->RegulAdminVi($vi, $jiaby);

    
} else if (empty($_SESSION["admin"]) && !empty($_SESSION["vi"]) && !empty($admin) && !empty($vi)) {
    
    // $this->RegulAdminVi($vi, $admin);
    $v= array();
    foreach ($_SESSION["vi"] as $item) {
        $v[]= get_object_vars($item);
    }

    $vi_merge= array_merge($v, $vi);
    $this->RegulAdminVi($vi_merge, $admin);

} else if (!empty($_SESSION["admin"]) && !empty($_SESSION["vi"]) && !empty($admin) && !empty($vi)) {
    $v= array();
    foreach ($_SESSION["vi"] as $item) {
        $v[]= get_object_vars($item);
    }

    $adjust= array();
    foreach ($_SESSION["admin"] as $item) {
        $adjust[]= get_object_vars($item);
    }

    $vi_merge= array($v, $vi);
    $adjust_merge = array($adjust, $admin);
    $this->RegulAdminVi($vi_merge, $adjust_merge);
    
} else if (empty($_SESSION["admin"]) && empty($_SESSION["vi"]) && !empty($admin) && !empty($vi)) {
    $this->RegulAdminVi($vi, $admin);
} else if (empty($_SESSION["admin"]) && !empty($_SESSION["vi"]) && !empty($admin) && empty($vi)) {
    $v= array();
    foreach ($_SESSION["vi"] as $item) {
        $v[]= get_object_vars($item);
    }
    $this->RegulAdminVi($v, $admin);
} else if (!empty($_SESSION["admin"]) && !empty($_SESSION["vi"]) && !empty($admin) && empty($vi)) {

    $v= array();
    foreach ($_SESSION["vi"] as $item) {
        $v[]= get_object_vars($item);
    }
    $adjust= array();
    foreach($_SESSION["admin"] as $item) {
        $adjust[]= get_object_vars($item);
    }

    $adjust_merge= array_merge($admin, $adjust);
    $this->RegulAdminVi($v, $adjust_merge);



    
} else if (!empty($_SESSION["admin"]) && empty($_SESSION["vi"]) && !empty($admin) && empty($vi)) {

    foreach($admin as $item) {
        $this->Telma_Anomalie_Model->insert_or_update_admin($item);
    }
} else if (empty($_SESSION["admin"]) && !empty($_SESSION["vi"]) && empty($admin) && !empty($vi)) {
    foreach($vi as $item) {
        $this->Boa_Telma_Anomalie_Model->insert_or_update_vi($item);
    }
}


// **************************************************************************************** INSERTION DES DONNEES *********************************************************************************//

                    $_SESSION["last_solde"] = $this->Historique_Telma_Model->get_last_solde();
                    if(count($_SESSION["last_solde"]) > 0 ) {
                        $solde_telma= $_SESSION["last_solde"][0]->solde_telma;
                        $solde_boa= $_SESSION["last_solde"][0]->solde_boa;
                    } else if(count($_SESSION["last_solde"]) === 0 ) {
                        $solde_telma= 0;
                        $solde_boa= 0;
                    }

                    $historique =array_merge($dat[0], $cat[0], $reverseEtAnnule, $admin, $nonAu, $mvtsCI[0], $mvtsCO[0], $vi ,$normalCI[0], $normalCO[0]);
                    
                    foreach ($historique as $item) {
                        $solde_telma += isset($item["solde"]) ? $item["solde"] : 0;
                        $solde_boa += isset($item["MONTANT"]) ? $item["MONTANT"] : 0;
                        $item["solde_telma"] = $solde_telma;
                        $item["solde_boa"] = $solde_boa;
                        $this->Historique_Telma_Model->insert_or_update($item);
                    }

                    foreach ($normalCI[0] as $item) {
                        $this->Telma_Normal_Model->insert_or_update_ci($item);
                    }

                    foreach ($normalCO[0] as $item) {
                        $this->Telma_Normal_Model->insert_or_update_co($item);
                    }

                    foreach($reverseEtAnnule as $item) {
                        $this->Telma_Anomalie_Model->insert_or_update_reverse($item);
                            
                    }
                    foreach($rollback as $item) {
                        $this->Telma_Anomalie_Model->insert_or_update_rollback($item);
                    
                    }

                    foreach($mvtsCI[0] as $item) {
                        $this->Telma_Anomalie_Model->insert_or_update_mvts_ci($item);
                    }

                    foreach($mvtsCO[0] as $item) {
                        $this->Telma_Anomalie_Model->insert_or_update_mvts_co($item);
                    }

                    foreach($dat[0] as $item) {
                        $item["etat"] = "Non";
                        $this->Boa_Telma_Anomalie_Model->insert_or_update_dat($item);
                    
                    }
                  
                    foreach($cat[0] as $item) {
                        $item["etat"] = "Non";
                        $this->Boa_Telma_Anomalie_Model->insert_or_update_cat($item);
                    
                    }
                     
                    $newNonAu = $this->changerCle($nonAu);
                    foreach($newNonAu as $item) {
                        $this->Boa_Telma_Anomalie_Model->insert_or_update_nonAu($item);
                    }              

// *******************************************************************************************************************************

                $this->exporter($normalCI[0], $normalCO[0], $reverseEtAnnule,$admin, $rollback, $dat[0], $cat[0], $vi, $nonAu, $mvtsCI[0], $mvtsCO[0]);
                }
                // redirect("importer_telma");
            }
        }

        public function readData($upload_file, $upload_tmp) {
            $extension = pathinfo($upload_file, PATHINFO_EXTENSION);

            if($extension ==="csv") {
                $reader= new \PhpOffice\PhpSpreadsheet\Reader\Csv();

            } else if ($extension ==="xls") {
                $reader= new \PhpOffice\PhpSpreadsheet\Reader\Xls();
            } else {
                $reader=  new \PhpOffice\PhpSpreadsheet\Reader\Xlsx();
            }

            $spreadsheet= $reader->load($upload_tmp);
            $sheetdata= $spreadsheet->getActiveSheet()->toArray();

            return $sheetdata;

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

        public function filtrerVI($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["OPER"] ==="VI") {
                    $item["etat"] = "Non";
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }

        public function filtrerAdmin($data) {
            $resultat= array();

            foreach($data as $item) {
                if($item["TYPE"] ==="adjustment" && $item["State"] === "Completed") {
                    $item["etat"] = "Non";
                    $item["solde"] = $item["Amount_MGA"] * 1;
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }

        public function filtrerReverse($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["TYPE"] ==="reverse") {
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }

        public function filtrerRollBack($data) {
            $resultat= array();

            // foreach($data as $item) {
            //     if($item["TYPE"] ==="transfer" && $item["Channel"] ==="SOAP" && $item["State"] ==="RolledBack") {
            //         $item["solde"] = $item["Amount_MGA"] * -1;
            //         $resultat[]= $item;
            //     } 
            // }
            foreach($data as $item) {
                if($item["State"] ==="RolledBack") {
                    $item["solde"] = $item["Amount_MGA"] * -1;
                    $resultat[]= $item;
                }
            }

            return $resultat;
        }

        public function RegulViAdmin($vi, $adjust) {
            $viCopy= $vi;
            $adjustCopy= $adjust;

            foreach($adjust as $itemAdjust) {
                $i=0;
                foreach($vi as $itemVi) {
                    if($itemAdjust["Amount_MGA"] == -$itemVi["MONTANT"]) {
                        $adjustCopy[$i]["etat"]= "Oui";
                        $adjustCopy[$i]["REF_IGOR"] = $itemVi["REF_IGOR"];
                    }
                }
                $i++;
            }

            return [$vi, $adjustCopy];
        }

        public function RegulAdminVi($vi, $adjust) {
            $viCopy= $vi;
            $adjustCopy= $adjust;

                foreach($vi as $i=>$itemVi) {
                    foreach($adjust as $j=>$itemAdjust) {
                        if($itemVi["MONTANT"] == -$itemAdjust["Amount_MGA"]) {
                            $vi[$i]["etat"]= "Oui";
                            $vi[$i]["trans_id"] = $itemAdjust["trans_id"];
                            unset($adjust[$j]);
                            break;
                        }
                    }
                }
            
            foreach($adjustCopy as $item) {
                $this->Telma_Anomalie_Model->insert_or_update_admin($item);
            }
        
            foreach($vi as $item) {
                $this->Boa_Telma_Anomalie_Model->insert_or_update_vi($item);
            } 
            //return [$vi, $adjustCopy];
        }

        // public function filtrerSoapEtCompltedEtTransfer($data) {
        //     $resultat= array();

        //     foreach($data as $item) {
        //         if($item["TYPE"] ==="transfer" && $item["Channel"] ==="SOAP" && $item["State"] ==="Completed") {
        //             $resultat[]= $item;
        //         }
        //     }

        //     return $resultat;
        // }

        public function filtrerCI($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["ACTION"] ==="bank_to_wallet") {
                    $item["cle"] = $item["receiver"] .$item["Amount_MGA"];
                    $item["solde"] = $item["Amount_MGA"] * -1;
                    $item["telma_heure"] = substr($item["date_d"], 10);
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }

        public function filtrerCO($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["ACTION"] ==="wallet_to_bank") {
                    $item["cle"] = $item["Sender"] .$item["Amount_MGA"];
                    $item["solde"] = $item["Amount_MGA"] * 1;
                    $item["telma_heure"] = substr($item["date_d"], 10);
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }

        public function trierParCleEtHeure($data) {

            usort($data, function ($a, $b) {
                $cleA = intval($a['cle']);
                $cleB = intval($b['cle']);

                if ($cleA == $cleB) {

                    $heureA = isset($a['telma_heure']) ? $a['telma_heure'] : $a['BOA_HEURE'];
                    $heureB = isset($b['telma_heure']) ? $b['telma_heure'] : $b['BOA_HEURE'];

                    return strcmp($heureA, $heureB);
                }
                return ($cleA < $cleB) ? -1 : 1;
            });
            return $data;
        }

        public function trierParDate($data) {
            usort($data, function ($a, $b) {
                $dateA = DateTime::createFromFormat('d/m/YH:i:s', $a['date_d']);
                $dateB = DateTime::createFromFormat('d/m/YH:i:s', $b['date_d']);
        
                if ($dateA === $dateB) {
                    return 0;
                }
            return ($dateA < $dateB) ? -1 : 1;
            });
        
            return $data;
        }
        

        public function trierParTransId($data) {
            usort($data, function($a, $b) {
                return $a['trans_id'] - $b['trans_id'];
            });
            return $data;
        }

        public function trouverIndexEtCleReverse($data) {
            $indexesEtCleReverse = [];
            $resultat= array();
            foreach ($data as $index => $element) {
                if ($element['TYPE'] === 'reverse') {
                    $indexesEtCleReverse["index"] = $index;
                    $indexesEtCleReverse["receiver"] = $element["receiver"];
                    $resultat[]= $indexesEtCleReverse;

                }
            }
            return $resultat;
        }

        public function trouverAnnulation($data, $indexesEtCleReverse) {

            $resultatIndex = array();
            $indexAnnul;

            foreach($indexesEtCleReverse as $itemCle) {

                $i=0;
                $indexAnnul =0;
                for ($i ; $i < $itemCle["index"]  ; $i++) {
                    if($data[$i]["initiator"] === $itemCle["receiver"]) {
                        $indexAnnul=$i;
                    } 
                }
                $resultatIndex[] = $indexAnnul;
                $resultatIndex[] = $itemCle["index"];
            }

            // $resultatIndex[]= array_merge($resultatIndex, $indexesEtCleReverse["index"]);

            return $resultatIndex;
        }


        public function trouverIndexAdmin($data) {
            
            $index= array();
            $i=0;

            for ($i=0; $i <count($data); $i++) {
                if($data[$i]["TYPE"] ==="adjustment") {
                    $index[]= $i;
                }
            }
            return $index;
        }

        public function trouverIndexRollBack($data) {
            $index= array();
            $i=0;

            for ($i=0; $i <count($data); $i++) {
                if($data[$i]["TYPE"] ==="transfer" && $data[$i]["Channel"] === "SOAP" && $data[$i]["State"] ==="RolledBack") {
                    $index[]= $i;
                }
            }
            return $index;
        }

        public function supprimerIndices($telma, $indicesASupprimer) {
            foreach ($indicesASupprimer as $indice) {
                if (isset($telma[$indice])) {
                    unset($telma[$indice]);
                }
            }
            return array_values($telma);
        }


        public function filtrerCIIgor($data) {

            $resultat= array();
            foreach($data as $item) {
                if($item["EXPL"] ==="AU" && $item["OPER"] ==="CASHI") {
                    $libelle= $item["LIBELLE"];
                    if (preg_match('/tel:(\d+)/', $libelle, $matches)) {
                        $numeroTelephone = $matches[1];
                        $item["BOA_HEURE"]= substr($item["LIBELLE"], -8);
                        $item["cle"] = $numeroTelephone .$item["MONTANT"]; 
                        $resultat[]= $item;                       
                    }
                }
            }
            return $resultat;
        }

        public function filtrerCOIgor($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["EXPL"] ==="AU" && $item["OPER"] ==="CASHO") {
                    $libelle= $item["LIBELLE"];
                    if (preg_match('/tel:(\d+)/', $libelle, $matches)) {
                        $numeroTelephone = $matches[1];
                        $item["BOA_HEURE"]= substr($item["LIBELLE"], -8);
                        $item["cle"] = $numeroTelephone .-($item["MONTANT"]); 
                        $resultat[]= $item;                       
                    }
                }
            }
            return $resultat;
        }

        public function filtrerNonAu($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["EXPL"] !=="AU" && $item["OPER"] !== "VI") {
                    $resultat[]= $item;                       
                }
            }
            return $resultat;
        }

        public function estTrieParCle($data) {
            $longueur = count($data);
            
            for ($i = 0; $i < $longueur - 1; $i++) {
                if ($data[$i]['cle'] > $data[$i + 1]['cle']) {
                    return false; // Le tableau n'est pas trié
                }
            }
            
            return true; 
        }

        public function changerCle($data) {
            $resultat= array();
            foreach ($data as &$item) {
                $item = array_change_key_case($item, CASE_LOWER);
                $resultat[]= $item;
            }

            return $resultat;
        }

        public function exporter($normalCI, $normalCO, $reverseEtAnnule, $admin, $rollback, $dat, $cat, $vi, $nonAu, $mvtsCI, $mvtsCO) {
            $dateAujourdhui = date("Y-m-d");
            $nomFichier = "RapproTelma-" .$dateAujourdhui .".xlsx";
            
            header("Content-Type: application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
            header("Content-Disposition: attachment; filename=\"" . $nomFichier . "\"");
            
            $spreadsheet = new PhpOffice\PhpSpreadsheet\Spreadsheet();
            $sheet = $spreadsheet->getActiveSheet();

            $cell_array = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "AA", "AB", "AC", "AD", "AE", "AF", "AG", "AH", "AI", "AJ", "AK"];
            $header = ["DATE OPE", "DATE VAL", "DEVISE", "MONTANT", "LIBELLE", "CODE OPER", "EXPL", "REF", "", "REF2", "SOLDE", "", "date_d", "trans_id", "initiator", "TYPE", "Channel", "State", "Wallet", "Amount_MGA", "Sender", "Sender_name", "receiver", "receiver_name", "details1", "Confirming_agent", "notify_alternate", "Origine", "TVA", "ACTION", "AA1 GROUP", "PAR", "MONTANT", "SOLDE", "ECART", "SOLDE DISPO", "SOLDE TELMA SALFECARE"];

            $sheet->mergeCells('A1:K1');
            $sheet->setCellValue('A1', 'BOA');
            $sheet->getStyle('A1')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('A1:K1')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->mergeCells('M1:AH1');
            $sheet->setCellValue('M1', 'Telma');
            $sheet->getStyle('M1')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('M1:AH1')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->mergeCells('A2:K2');
            $sheet->mergeCells('M2:AH2');
            $sheet->setCellValue('A2', 'PRINCIPALE');
            $sheet->getStyle('A2')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('A2:K2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->getStyle('M2:AH2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->getStyle('A1')->getFont()->setBold(true);
            $sheet->getStyle('M1')->getFont()->setBold(true);
            $sheet->getStyle('A2')->getFont()->setBold(true);
            $sheet->getStyle('E3')->getFont()->setBold(true);

            
            //  BORDURE 
            for($i=0; $i < count($cell_array) -3 ; $i++) {
                $sheet->getStyle($cell_array[$i] . "3")->getBorders()->getAllBorders()->setBorderStyle('thin');
            }

            // EN TETE 

            for($i=0; $i < count($cell_array) ; $i++) {
                $sheet->getStyle($cell_array[$i] . "4")->getBorders()->getAllBorders()->setBorderStyle('thin');
                $sheet->setCellValue($cell_array[$i] ."4", $header[$i]);
                $sheet->getStyle($cell_array[$i] . "4")->getAlignment()->setHorizontal('center');
                $sheet->getStyle($cell_array[$i] . "4")->getFont()->setBold(true);
                $sheet->getColumnDimension($cell_array[$i])->setAutoSize(true);
            }

            for($i=0; $i < count($cell_array) ; $i++) {
                $sheet->getStyle($cell_array[$i] . "4")->getBorders()->getAllBorders()->setBorderStyle('thin');
                $sheet->setCellValue($cell_array[$i] ."4", $header[$i]);
                $sheet->getStyle($cell_array[$i] . "4")->getAlignment()->setHorizontal('center');
                $sheet->getStyle($cell_array[$i] . "4")->getFont()->setBold(true);
                $sheet->getColumnDimension($cell_array[$i])->setAutoSize(true);
            }

            
            // NORMALE CI 

            $lastRow = $sheet->getHighestRow() + 2;
            foreach ($normalCI as $dataRow) {
                $sheet->getStyle($cell_array[0] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->setCellValue($cell_array[0] .$lastRow, $dataRow["DATE_OPER"]);
                $sheet->setCellValue($cell_array[1] .$lastRow, $dataRow["DATE_VAL"]);
                $sheet->setCellValue($cell_array[2] .$lastRow, $dataRow["DEVISE"]);
                $sheet->setCellValue($cell_array[3] .$lastRow, $dataRow["MONTANT"]);
                $sheet->setCellValue($cell_array[4] .$lastRow, $dataRow["LIBELLE"]);
                $sheet->setCellValue($cell_array[5] .$lastRow, $dataRow["OPER"]);
                $sheet->setCellValue($cell_array[6] .$lastRow, $dataRow["EXPL"]);
                $sheet->setCellValue($cell_array[7] .$lastRow, $dataRow["REF_IGOR"]);
                $sheet->setCellValue($cell_array[9] .$lastRow, $dataRow["cle"]);
                $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow["date_d"]);
                $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow["trans_id"]);
                $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow["initiator"]);
                $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow["TYPE"]);
                $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow["Channel"]);
                $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow["State"]);
                $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow["Wallet"]);
                $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow["Amount_MGA"]);
                $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow["Sender"]);
                $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow["Sender_name"]);
                $sheet->setCellValue($cell_array[22] .$lastRow, $dataRow["receiver"]);
                $sheet->setCellValue($cell_array[23] .$lastRow, $dataRow["receiver_name"]);
                $sheet->setCellValue($cell_array[24] .$lastRow, $dataRow["details1"]);
                $sheet->setCellValue($cell_array[25] .$lastRow, $dataRow["Confirming_agent"]);
                $sheet->setCellValue($cell_array[26] .$lastRow, $dataRow["origine"]);
                $sheet->setCellValue($cell_array[27] .$lastRow, $dataRow["TVA"]);
                $sheet->setCellValue($cell_array[29] .$lastRow, $dataRow["ACTION"]);
                $sheet->setCellValue($cell_array[30] .$lastRow, $dataRow["AA1_GROUP"]);
                $sheet->setCellValue($cell_array[31] .$lastRow, $dataRow["PAR"]);
                $sheet->setCellValue($cell_array[32] .$lastRow, $dataRow["solde"]);

                $lastRow++;
            }


            $lastRow = $sheet->getHighestRow() + 2;

            // NORMALE CO 

            foreach ($normalCO as $dataRow) {
                $sheet->getStyle($cell_array[0] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->setCellValue($cell_array[0] .$lastRow, $dataRow["DATE_OPER"]);
                $sheet->setCellValue($cell_array[1] .$lastRow, $dataRow["DATE_VAL"]);
                $sheet->setCellValue($cell_array[2] .$lastRow, $dataRow["DEVISE"]);
                $sheet->setCellValue($cell_array[3] .$lastRow, $dataRow["MONTANT"]);
                $sheet->setCellValue($cell_array[4] .$lastRow, $dataRow["LIBELLE"]);
                $sheet->setCellValue($cell_array[5] .$lastRow, $dataRow["OPER"]);
                $sheet->setCellValue($cell_array[6] .$lastRow, $dataRow["EXPL"]);
                $sheet->setCellValue($cell_array[7] .$lastRow, $dataRow["REF_IGOR"]);
                $sheet->setCellValue($cell_array[9] .$lastRow, $dataRow["cle"]);
                $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow["date_d"]);
                $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow["trans_id"]);
                $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow["initiator"]);
                $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow["TYPE"]);
                $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow["Channel"]);
                $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow["State"]);
                $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow["Wallet"]);
                $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow["Amount_MGA"]);
                $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow["Sender"]);
                $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow["Sender_name"]);
                $sheet->setCellValue($cell_array[22] .$lastRow, $dataRow["receiver"]);
                $sheet->setCellValue($cell_array[23] .$lastRow, $dataRow["receiver_name"]);
                $sheet->setCellValue($cell_array[24] .$lastRow, $dataRow["details1"]);
                $sheet->setCellValue($cell_array[25] .$lastRow, $dataRow["Confirming_agent"]);
                $sheet->setCellValue($cell_array[26] .$lastRow, $dataRow["origine"]);
                $sheet->setCellValue($cell_array[27] .$lastRow, $dataRow["TVA"]);
                $sheet->setCellValue($cell_array[29] .$lastRow, $dataRow["ACTION"]);
                $sheet->setCellValue($cell_array[30] .$lastRow, $dataRow["AA1_GROUP"]);
                $sheet->setCellValue($cell_array[31] .$lastRow, $dataRow["PAR"]);
                $sheet->setCellValue($cell_array[32] .$lastRow, $dataRow["solde"]);

                $lastRow++;
            }

            // REVERSE ET ANNULE 

            $lastRow = $sheet->getHighestRow() + 3;
            
            foreach ($reverseEtAnnule as $dataRow) {
                $sheet->getStyle($cell_array[12] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[13] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[14] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[15] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[16] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[17] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[18] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[19] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[20] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[21] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[22] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[23] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[24] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[25] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[26] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[27] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[29] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[30] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[31] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');
                $sheet->getStyle($cell_array[32] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('D3D3D3');



                $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow["date_d"]);
                $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow["trans_id"]);
                $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow["initiator"]);
                $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow["TYPE"]);
                $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow["Channel"]);
                $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow["State"]);
                $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow["Wallet"]);
                $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow["Amount_MGA"]);
                $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow["Sender"]);
                $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow["Sender_name"]);
                $sheet->setCellValue($cell_array[22] .$lastRow, $dataRow["receiver"]);
                $sheet->setCellValue($cell_array[23] .$lastRow, $dataRow["receiver_name"]);
                $sheet->setCellValue($cell_array[24] .$lastRow, $dataRow["details1"]);
                $sheet->setCellValue($cell_array[25] .$lastRow, $dataRow["Confirming_agent"]);
                $sheet->setCellValue($cell_array[26] .$lastRow, $dataRow["origine"]);
                $sheet->setCellValue($cell_array[27] .$lastRow, $dataRow["TVA"]);
                $sheet->setCellValue($cell_array[29] .$lastRow, $dataRow["ACTION"]);
                $sheet->setCellValue($cell_array[30] .$lastRow, $dataRow["AA1_GROUP"]);
                $sheet->setCellValue($cell_array[31] .$lastRow, $dataRow["PAR"]);
                $sheet->setCellValue($cell_array[32] .$lastRow, $dataRow["solde"]);

                $lastRow++;
            }
            // ROLLBACK 

            $lastRow = $sheet->getHighestRow() + 2;

            foreach ($rollback as $dataRow) {
                $sheet->getStyle($cell_array[12] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[13] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[14] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[15] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[16] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[17] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[18] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[19] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[20] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[21] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[22] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[23] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[24] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[25] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[26] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[27] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[29] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[30] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[31] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');
                $sheet->getStyle($cell_array[32] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('8B4513');

                $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow["date_d"]);
                $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow["trans_id"]);
                $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow["initiator"]);
                $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow["TYPE"]);
                $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow["Channel"]);
                $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow["State"]);
                $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow["Wallet"]);
                $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow["Amount_MGA"]);
                $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow["Sender"]);
                $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow["Sender_name"]);
                $sheet->setCellValue($cell_array[22] .$lastRow, $dataRow["receiver"]);
                $sheet->setCellValue($cell_array[23] .$lastRow, $dataRow["receiver_name"]);
                $sheet->setCellValue($cell_array[24] .$lastRow, $dataRow["details1"]);
                $sheet->setCellValue($cell_array[25] .$lastRow, $dataRow["Confirming_agent"]);
                $sheet->setCellValue($cell_array[26] .$lastRow, $dataRow["origine"]);
                $sheet->setCellValue($cell_array[27] .$lastRow, $dataRow["TVA"]);
                $sheet->setCellValue($cell_array[29] .$lastRow, $dataRow["ACTION"]);
                $sheet->setCellValue($cell_array[30] .$lastRow, $dataRow["AA1_GROUP"]);
                $sheet->setCellValue($cell_array[31] .$lastRow, $dataRow["PAR"]);
                $sheet->setCellValue($cell_array[32] .$lastRow, $dataRow["solde"]);

                $lastRow++;
            }

            $lastRow = $sheet->getHighestRow() + 2;
            
            foreach ($admin as $dataRow) {
                $sheet->getStyle($cell_array[12] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[13] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[14] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[15] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[16] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[17] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[18] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[19] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[20] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[21] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[22] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[23] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[24] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[25] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[26] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[27] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[29] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[30] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[31] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[32] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');


                $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow["date_d"]);
                $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow["trans_id"]);
                $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow["initiator"]);
                $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow["TYPE"]);
                $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow["Channel"]);
                $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow["State"]);
                $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow["Wallet"]);
                $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow["Amount_MGA"]);
                $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow["Sender"]);
                $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow["Sender_name"]);
                $sheet->setCellValue($cell_array[22] .$lastRow, $dataRow["receiver"]);
                $sheet->setCellValue($cell_array[23] .$lastRow, $dataRow["receiver_name"]);
                $sheet->setCellValue($cell_array[24] .$lastRow, $dataRow["details1"]);
                $sheet->setCellValue($cell_array[25] .$lastRow, $dataRow["Confirming_agent"]);
                $sheet->setCellValue($cell_array[26] .$lastRow, $dataRow["origine"]);
                $sheet->setCellValue($cell_array[27] .$lastRow, $dataRow["TVA"]);
                $sheet->setCellValue($cell_array[29] .$lastRow, $dataRow["ACTION"]);
                $sheet->setCellValue($cell_array[30] .$lastRow, $dataRow["AA1_GROUP"]);
                $sheet->setCellValue($cell_array[31] .$lastRow, $dataRow["PAR"]);
                $sheet->setCellValue($cell_array[32] .$lastRow, $dataRow["solde"]);

                $lastRow++;
            }

            $lastRow = $sheet->getHighestRow() + 0; 
            foreach ($vi as $dataRow) {
                $sheet->getStyle($cell_array[0] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[1] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[2] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[3] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[3] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[4] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[5] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[6] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[7] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[8] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[9] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');

                $sheet->setCellValue($cell_array[0] .$lastRow, $dataRow["DATE_OPER"]);
                $sheet->setCellValue($cell_array[1] .$lastRow, $dataRow["DATE_VAL"]);
                $sheet->setCellValue($cell_array[2] .$lastRow, $dataRow["DEVISE"]);
                $sheet->setCellValue($cell_array[3] .$lastRow, $dataRow["MONTANT"]);
                $sheet->setCellValue($cell_array[4] .$lastRow, $dataRow["LIBELLE"]);
                $sheet->setCellValue($cell_array[5] .$lastRow, $dataRow["OPER"]);
                $sheet->setCellValue($cell_array[6] .$lastRow, $dataRow["EXPL"]);
                $sheet->setCellValue($cell_array[7] .$lastRow, $dataRow["REF_IGOR"]);


                $lastRow++;
            }

            // DEBIT A TORT
            $lastRow = $sheet->getHighestRow() + 2; 
            foreach ($dat as $dataRow) {
                $sheet->getStyle($cell_array[0] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[1] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[2] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[3] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[3] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[4] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[5] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[6] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[7] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[8] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[9] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');

                $sheet->setCellValue($cell_array[0] .$lastRow, $dataRow["DATE_OPER"]);
                $sheet->setCellValue($cell_array[1] .$lastRow, $dataRow["DATE_VAL"]);
                $sheet->setCellValue($cell_array[2] .$lastRow, $dataRow["DEVISE"]);
                $sheet->setCellValue($cell_array[3] .$lastRow, $dataRow["MONTANT"]);
                $sheet->setCellValue($cell_array[4] .$lastRow, $dataRow["LIBELLE"]);
                $sheet->setCellValue($cell_array[5] .$lastRow, $dataRow["OPER"]);
                $sheet->setCellValue($cell_array[6] .$lastRow, $dataRow["EXPL"]);
                $sheet->setCellValue($cell_array[7] .$lastRow, $dataRow["REF_IGOR"]);
                $sheet->setCellValue($cell_array[9] .$lastRow, $dataRow["cle"]);

                $lastRow++;
            }

            // CREDIT A TORT 

            $lastRow = $sheet->getHighestRow() + 2; 
            foreach ($cat as $dataRow) {
                $sheet->getStyle($cell_array[0] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[1] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[2] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[3] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[3] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[4] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[5] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[6] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[7] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[8] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[9] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');

                $sheet->setCellValue($cell_array[0] .$lastRow, $dataRow["DATE_OPER"]);
                $sheet->setCellValue($cell_array[1] .$lastRow, $dataRow["DATE_VAL"]);
                $sheet->setCellValue($cell_array[2] .$lastRow, $dataRow["DEVISE"]);
                $sheet->setCellValue($cell_array[3] .$lastRow, $dataRow["MONTANT"]);
                $sheet->setCellValue($cell_array[4] .$lastRow, $dataRow["LIBELLE"]);
                $sheet->setCellValue($cell_array[5] .$lastRow, $dataRow["OPER"]);
                $sheet->setCellValue($cell_array[6] .$lastRow, $dataRow["EXPL"]);
                $sheet->setCellValue($cell_array[7] .$lastRow, $dataRow["REF_IGOR"]);
                $sheet->setCellValue($cell_array[9] .$lastRow, $dataRow["cle"]);

                $lastRow++;
            }

            $lastRow = $sheet->getHighestRow() + 2;

            foreach ($mvtsCI as $dataRow) {
                $sheet->getStyle($cell_array[12] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[13] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[14] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[15] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[16] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[17] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[18] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[19] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[20] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[21] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[22] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[23] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[24] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[25] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[26] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[27] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[29] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[30] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[31] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[32] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');

                $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow["date_d"]);
                $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow["trans_id"]);
                $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow["initiator"]);
                $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow["TYPE"]);
                $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow["Channel"]);
                $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow["State"]);
                $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow["Wallet"]);
                $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow["Amount_MGA"]);
                $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow["Sender"]);
                $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow["Sender_name"]);
                $sheet->setCellValue($cell_array[22] .$lastRow, $dataRow["receiver"]);
                $sheet->setCellValue($cell_array[23] .$lastRow, $dataRow["receiver_name"]);
                $sheet->setCellValue($cell_array[24] .$lastRow, $dataRow["details1"]);
                $sheet->setCellValue($cell_array[25] .$lastRow, $dataRow["Confirming_agent"]);
                $sheet->setCellValue($cell_array[26] .$lastRow, $dataRow["origine"]);
                $sheet->setCellValue($cell_array[27] .$lastRow, $dataRow["TVA"]);
                $sheet->setCellValue($cell_array[29] .$lastRow, $dataRow["ACTION"]);
                $sheet->setCellValue($cell_array[30] .$lastRow, $dataRow["AA1_GROUP"]);
                $sheet->setCellValue($cell_array[31] .$lastRow, $dataRow["PAR"]);
                $sheet->setCellValue($cell_array[32] .$lastRow, $dataRow["solde"]);

                $lastRow++;
            }

            $lastRow = $sheet->getHighestRow() + 2;

            foreach ($mvtsCO as $dataRow) {
                $sheet->getStyle($cell_array[12] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[13] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[14] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[15] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[16] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[17] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[18] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[19] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[20] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[21] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[22] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[23] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[24] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[25] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[26] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[27] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[29] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[30] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[31] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[32] .$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');

                $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow["date_d"]);
                $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow["trans_id"]);
                $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow["initiator"]);
                $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow["TYPE"]);
                $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow["Channel"]);
                $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow["State"]);
                $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow["Wallet"]);
                $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow["Amount_MGA"]);
                $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow["Sender"]);
                $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow["Sender_name"]);
                $sheet->setCellValue($cell_array[22] .$lastRow, $dataRow["receiver"]);
                $sheet->setCellValue($cell_array[23] .$lastRow, $dataRow["receiver_name"]);
                $sheet->setCellValue($cell_array[24] .$lastRow, $dataRow["details1"]);
                $sheet->setCellValue($cell_array[25] .$lastRow, $dataRow["Confirming_agent"]);
                $sheet->setCellValue($cell_array[26] .$lastRow, $dataRow["origine"]);
                $sheet->setCellValue($cell_array[27] .$lastRow, $dataRow["TVA"]);
                $sheet->setCellValue($cell_array[29] .$lastRow, $dataRow["ACTION"]);
                $sheet->setCellValue($cell_array[30] .$lastRow, $dataRow["AA1_GROUP"]);
                $sheet->setCellValue($cell_array[31] .$lastRow, $dataRow["PAR"]);
                $sheet->setCellValue($cell_array[32] .$lastRow, $dataRow["solde"]);

                $lastRow++;
            }

            
            $lastRow = $sheet->getHighestRow() + 2; 
            foreach ($nonAu as $dataRow) {
                $sheet->getStyle($cell_array[0] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[1] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[2] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[3] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[3] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[4] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[5] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[6] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[7] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[8] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[9] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');

                 
                $sheet->setCellValue($cell_array[0] .$lastRow, $dataRow["DATE_OPER"]);
                $sheet->setCellValue($cell_array[1] .$lastRow, $dataRow["DATE_VAL"]);
                $sheet->setCellValue($cell_array[2] .$lastRow, $dataRow["DEVISE"]);
                $sheet->setCellValue($cell_array[3] .$lastRow, $dataRow["MONTANT"]);
                $sheet->setCellValue($cell_array[4] .$lastRow, $dataRow["LIBELLE"]);
                $sheet->setCellValue($cell_array[5] .$lastRow, $dataRow["OPER"]);
                $sheet->setCellValue($cell_array[6] .$lastRow, $dataRow["EXPL"]);
                $sheet->setCellValue($cell_array[7] .$lastRow, $dataRow["REF_IGOR"]);


                $lastRow++;
            }


            $lastRow = $sheet->getHighestRow() +1;

            for ($row = 6; $row <= $lastRow; $row++) {
                $sheet->getStyle("K" . $row)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFFF00');
                $sheet->setCellValue("K$row", "=D$row + K" . ($row - 1));
            }

            for ($row = 6; $row <= $lastRow; $row++) {
                $sheet->setCellValue("AH$row", "=AG$row + AH" . ($row - 1));
            }
           

            for ($row = 6; $row <= $lastRow; $row++) {
                $sheet->setCellValue("AI$row", "=D$row + AG$row");
            }

            $lastRow = $sheet->getHighestRow() +1;

            $sheet->getStyle("L6:L".$lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('00FF00');

            $lastRow = $sheet->getHighestRow();

            $sheet->setCellValue("AI5", "=SUM(AI6:AI$lastRow)");

            $writer = new PhpOffice\PhpSpreadsheet\Writer\Xlsx($spreadsheet);
            $writer->save('php://output');

        }


        public function comparerTelmaEtIgor ($igorTab, $telmaTab) {
            
            $telmaNormale = array();
            $igorNormale = array();
            $igorAnomalie = array();
            $telmaAnomalie = array();

            $telmaCount = count($telmaTab);
            $igorCount = count($igorTab);

            $i = 0;
            $j = 0;

            while ($i < $igorCount && $j < $telmaCount) {
                $cleIgor = $igorTab[$i]["cle"];
                $cleTelma = $telmaTab[$j]["cle"];
                $difference = $cleIgor - $cleTelma;

                if ($difference === 0) {
                    $heureIgor = substr($igorTab[$i]['LIBELLE'], -8);
                    $heureTelma = substr($telmaTab[$j]['date_d'], -8);
                    $timeDiff = strtotime($heureIgor) - strtotime($heureTelma);

                    // $telmaNormale[] = $telmaTab[$j];
                    // $igorNormale[] = $igorTab[$i];
                    // $i++;
                    // $j++;

                    // echo $timeDiff ."\n";
                    if($timeDiff < -360) {
                        $telmaNormale[] = $telmaTab[$j];
                        $igorAnomalie[] = $igorTab[$i];
                        $i++;
                    } else {
                        $telmaNormale[] = $telmaTab[$j];
                        $igorNormale[] = $igorTab[$i];
                        $i++;
                        $j++;
                    }

                   
                } elseif ($difference < 0) {
                    $igorAnomalie[] = $igorTab[$i];
                    $i++;
                } else {
                    $telmaAnomalie[] = $telmaTab[$j];
                    $j++;
                }
            }

           

            // Traiter les éléments restants dans $igorTab comme des anomalies
            while ($i < $igorCount) {
                $igorAnomalie[] = $igorTab[$i];
                $i++;
            }

            // Traiter les éléments restants dans $telmaTab comme des anomalies
            while ($j < $telmaCount) {
                $telmaAnomalie[] = $telmaTab[$j];
                $j++;
            }

            for($i=0; $i < count($igorNormale); $i++) {
                $mergeIgorTelma[]= array_merge($igorNormale[$i], $telmaNormale[$i]);
            } 

            return [[$mergeIgorTelma], [$igorAnomalie], [$telmaAnomalie]];
        }

        public function reverseEtAnnule($index, $telma) {
            $resultat = array();
            foreach($index as $item) {
                $element= $telma[$item];
                $element["solde"] = $telma[$item]["Amount_MGA"] * -1;
                $resultat[]= $element;
            }
            return $resultat;
        }
    }



?>