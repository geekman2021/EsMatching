


<div class=" container mt-5">

  
    <ul class="nav nav-tabs">
            <li class="nav-item col-sm-6">
                <a class="nav-link" href="#normale">Normale</a>
            </li>
            <li class="nav-item col-sm-6">
                <a class="nav-link" href="#anormale">Anomalie</a>
            </li>
    </ul>
</div>

  <div class="tab-content">
    <div id="normale" class="container tab-pane "><br>
      <ul class="nav nav-tabs">
        <li class="nav-item col-sm-6">
          <a class="nav-link" href="#tabNormaleCashIn">Normale Cash in</a>
        </li>
        <li class="nav-item col-sm-6">
          <a class="nav-link" href="#tabNormaleCashOut">Normale Cash Out</a>
        </li>
      </ul>
    </div>
    <div id="anormale" class="container tab-pane fade"><br>
      <ul class="nav nav-tabs">
        <li class="nav-item col-sm-3">
          <a class="nav-link" href="#tabDat">Débit à Tort</a>
        </li>
        <li class="nav-item col-sm-3">
          <a class="nav-link" href="#tabCat">Crédit à Tort</a>
        </li>
        <li class="nav-item col-sm-3">
          <a class="nav-link" href="#tabAdmin">Admin</a>
        </li>
        <li class="nav-item col-sm-3">
          <a class="nav-link" href="#tabVI">VI</a>
        </li>
      </ul>
    </div>

  </div>
  <div class="container tab-content">
    <div class="row">
      
    </div>
    
<div id="tabNormaleCashIn" class="tab-pane"><br>
    <div class="row">
      <div class="col-sm-6">
          <label for="dateDebut">Date Debut</label>
          <input type="text" class="form-control" id="min" name="min">
      </div>
      <div class="col-sm-6">
        <label for="dateDebut">Date Fin</label>
          <input type="text" class="form-control" id="max" name="max">
      </div> 
    </div>
        <div class="table-responsive mt-1">
            <table class="table table-bordered" id="tableNormaleCashIn">
                <thead style="text-align: center ;">
                    <tr>
                        <th colspan="9" style="text-align: center ;">BOA</th>
                        <th></th>
                        <th colspan="10" style="text-align: center ;">Telma</th>
                    </tr>
                    <tr> 
                        <th>Date_Oper</th>
                        <th>Date_Val</th>
                        <th>Heure</th>
                        <th>Montant</th>
                        <th>Devise</th>
                        <th>Libelle</th>
                        <th>Operation</th>
                        <th>Expl</th>
                        <th>ReferenceIgor</th>
                        <th>Numero</th>
                        <th>Date</th>
                        <th>Heure</th>
                        <th>Transfer_id</th>
                        <th>Type</th>
                        <th>Channel</th>
                        <th>Etat</th>
                        <th>Montant</th>
                        <th>sender</th>
                        <th>receiver</th>
                    </tr>
                </thead>
                <tbody>
                  <?php foreach($normal_ci as $item) { ?>
                    <tr>
                      <td><?php echo $item->DATE_OPER ?></td>
                      <td><?php echo $item->DATE_VAL ?></td>
                      <td><?php echo $item->BOA_HEURE ?></td>
                      <td><?php echo number_format($item->MONTANT, 0, ',', " ") ?></td>
                      <td><?php echo $item->DEVISE ?></td>
                      <td style="white-space: nowrap; overflow: hidden; text-overflow: ellipsis ;max-width: 100px; "><?php echo $item->LIBELLE ?></td>
                      <td><?php echo $item->OPER ?></td>
                      <td><?php echo $item->EXPL ?></td>
                      <td><?php echo $item->REF_IGOR ?></td>
                      <td><?php echo $item->cle ?></td>
                      <td><?php echo $item->date_d ?></td>
                      <td><?php echo $item->telma_heure ?></td>
                      <td><?php echo $item->trans_id ?></td>
                      <td><?php echo $item->TYPE ?></td>
                      <td><?php echo $item->channel ?></td>
                      <td><?php echo $item->state ?></td>
                      <td><?php echo number_format($item->Amount_MGA, 0, ',', " ") ?></td>
                      <td><?php echo $item->sender_name ?></td>
                      <td><?php echo $item->receiver_name ?></td>
                      
                    </tr>

                  <?php }?>

                    
                    
                </tbody>
            </table>
        </div>
    </div>



    <div id="tabNormaleCashOut" class="container tab-pane "><br>
        <!-- <div class="row">
          <div class="col-sm-6">
              <label for="dateDebut">Date Debut</label>
              <input type="text" class="form-control" id="min" name="min">
          </div>
          <div class="col-sm-6">
            <label for="dateDebut">Date Fin</label>
              <input type="text" class="form-control" id="max" name="max">
          </div> 
        </div> -->
        <div class="table-responsive">
            <table class="table table-bordered" id="tableNormaleCashOut">
            <thead style="text-align: center ;">
                    <tr>
                        <th colspan="9" style="text-align: center ;">BOA</th>
                        <th></th>
                        <th colspan="10" style="text-align: center ;">Telma</th>
                    </tr>
                    <tr>
                        
                      <th>Date_Oper</th>
                      <th>Date_Val</th>
                      <th>Heure</th>
                      <th>Montant</th>
                      <th>Devise</th>
                      <th>Libelle</th>
                      <th>Operation</th>
                      <th>Expl</th>
                      <th>ReferenceIgor</th>
                      <th>Numero</th>
                      <th>Date</th>
                      <th>Heure</th>
                      <th>Transfer_id</th>
                      <th>Type</th>
                      <th>Channel</th>
                      <th>Etat</th>
                      <th>Montant</th>
                      <th>sender</th>
                      <th>receiver</th>
                    </tr>
                </thead>
                <tbody>
                  <?php foreach($normal_co as $item) { ?>
                    <tr>
                    <td><?php echo $item->DATE_OPER ?></td>
                      <td><?php echo $item->DATE_VAL ?></td>
                      <td><?php echo $item->BOA_HEURE ?></td>
                      <td><?php echo number_format($item->MONTANT, 0, ',', " ") ?></td>
                      <td><?php echo $item->DEVISE ?></td>
                      <td style="white-space: nowrap; overflow: hidden; text-overflow: ellipsis ;max-width: 100px; "><?php echo $item->LIBELLE ?></td>
                      <td><?php echo $item->OPER ?></td>
                      <td><?php echo $item->EXPL ?></td>
                      <td><?php echo $item->REF_IGOR ?></td>
                      <td><?php echo substr($item->cle, 0, 10) ?></td>
                      <td><?php echo $item->date_d ?></td>
                      <td><?php echo $item->telma_heure ?></td>
                      <td><?php echo $item->trans_id ?></td>
                      <td><?php echo $item->TYPE ?></td>
                      <td><?php echo $item->channel ?></td>
                      <td><?php echo $item->state ?></td>
                      <td><?php echo number_format($item->Amount_MGA, 0, ',', " ") ?></td>
                      <td><?php echo $item->sender_name ?></td>
                      <td><?php echo $item->receiver_name ?></td>

                  <?php }?>

                    </tr>
                    
                </tbody>
            </table>
        </div>
    </div>
</div>

  <div class="container tab-content">
  <div id="tabDat" class="container tab-pane "><br>  
    <div class="table-responsive">
      <table class="table table-bordered" id="tableDat">
      <thead style="text-align: center ;">
        <tr>
            <th colspan="9" style="text-align: center ;">BOA</th>
            <th colspan="3" style="text-align: center ;">Régularisation</th>
        </tr>
        <tr>
            <th>Compte</th>
            <th>Date_Oper</th>
            <th>Date_Val</th>
            <th>Devise</th>
            <th>Montant</th>
            <th>Libelle</th>
            <th>Operation</th>
            <th>Expl</th>
            <th>ReferenceIgor</th>
            <th>Etat</th>
            <th>Date_Regul</th>
            <th>Action</th>
        </tr>
        </thead>
        <tbody>
        <?php foreach($dat as $item) { ?>
          <tr>
          <td><?php echo $item->COMPTE ?></td>
            <td><?php echo $item->DATE_OPER ?></td>
            <td><?php echo $item->DATE_VAL ?></td>
            <td><?php echo $item->DEVISE ?></td>
            <td><?php echo number_format($item->MONTANT, 0, ',', " ") ?></td>
            <td><?php echo $item->LIBELLE ?></td>
            <td><?php echo $item->OPER ?></td>
            <td><?php echo $item->EXPL ?></td>
            <td><?php echo $item->REF_IGOR ?></td>
            <td><?php echo $item->etat ?></td>
            <td><?php echo "" ?></td>
            <td style="text-align-center;">
              <a href="javascript:void(0);" class="text-warning" data-toggle="modal" data-target="#myModal" onClick="modifier('<?php echo $item->REF_IGOR ?>', '<?php echo $item->etat ?>', '<?php echo $item->date_regul ?>')">
              <i class="bx bx-edit" style="font-size: 20px;"></i>
              </a> 
              <a href="javascript:void(0);" class="text-primary" data-target="#myModalFr10"
                    data-compte="<?php echo $item->COMPTE ?>"
                    data-date-oper="<?php echo $item->DATE_OPER ?>"
                    data-montant="<?php echo $item->MONTANT ?>"
                    data-libelle="<?php echo $item->LIBELLE ?>"
                    data-oper="<?php echo $item->OPER ?>"
                    data-ref-igor="<?php echo $item->REF_IGOR ?>" data-toggle="modal">
                    <i class="bx bx-printer" style="font-size: 20px;"></i>
                </a>
            </td>
          </tr>
        <?php }?>
        </tbody>
      </table>
    </div>
  </div>
  <div id="tabCat" class="container tab-pane "><br>  
    <div class="table-responsive">
      <table class="table table-bordered" id="tableCat">
      <thead style="text-align: center ;">
        <tr>
            <th colspan="8" style="text-align: center ;">BOA</th>
            <th colspan="3" style="text-align: center ;">Régularisation</th>
        </tr>
        <tr>
            <th>Date_Oper</th>
            <th>Date_Val</th>
            <th>Devise</th>
            <th>Montant</th>
            <th>Libelle</th>
            <th>Operation</th>
            <th>Expl</th>
            <th>ReferenceIgor</th>
            <th>Etat</th>
            <th>Date_Regul</th>
            <th>Action</th>
        </tr>
        </thead>
        <tbody>
        <?php foreach($cat as $item) { ?>
          <tr>
            <td><?php echo $item->DATE_OPER ?></td>
            <td><?php echo $item->DATE_VAL ?></td>
            <td><?php echo $item->DEVISE ?></td>
            <td><?php echo number_format($item->MONTANT, 0, ',', " ") ?></td>
            <td><?php echo $item->LIBELLE ?></td>
            <td><?php echo $item->OPER ?></td>
            <td><?php echo $item->EXPL ?></td>
            <td><?php echo $item->REF_IGOR ?></td>
            <td><?php echo "" ?></td>
            <td><?php echo "" ?></td>
            <td><?php echo "" ?></td>
          </tr>

        <?php }?>
        </tbody>
      </table>
    </div>     
  </div>

  <div id="tabAdmin" class="container tab-pane "><br>    
    <div class="table-responsive">
      <table class="table table-bordered" id="tableAdmin">
        <thead>
          <tr>
            <th style="display: none;"></th>
            <th>Date</th>
            <!-- <th>Heure</th> -->
            <th>Transfer_id</th>
            <th>Type</th>
            <th>Channel</th>
            <th>Etat</th>
            <th>Montant</th>
            <th>sender</th>
            <th>receiver</th>
            <th>Etat</th>
            <th>Date_Regul</th>
            <th>Action</th>
          </tr>
        </thead>
        <tbody>
        <?php foreach($admin as $item) { ?>

          
          <tr>
            <td style="display: none;"><?php echo $item->id ?></td>
            <td><?php echo $item->date_d ?></td>
            <!-- <td><?php //echo $item->telma_heure ?></td> -->
            <td><?php echo $item->trans_id ?></td>
            <td><?php echo $item->TYPE ?></td>
            <td><?php echo $item->channel ?></td>
            <td><?php echo $item->state ?></td>
            <td><?php echo number_format($item->Amount_MGA, 0, ',', " ") ?></td>
            <td><?php echo $item->sender ?></td>
            <td><?php echo $item->receiver ?></td>
            <td><?php echo $item->etat ?></td>
            <td><?php echo $item->date_regul ?></td>
            <td>
              <a href="javascript:void(0);" class="text-warning" data-toggle="modal" data-target="#myModal" onClick="modifier('<?php echo $item->id ?>', '<?php echo $item->etat ?>', '<?php echo $item->date_regul ?>')">
                Modifier
              </a> 
            </td>
          </tr>

        <?php }?>
        </tbody>
      </table>
    </div>     
  </div>

  <div id="tabVI" class="tab-pane"><br>
      <div class="table-responsive">
          <table class="table table-bordered" id="tableVI">
              <thead style="text-align: center ;">
                  <tr>
                    <th>DateOper</th>
                    <th>DateVal</th>
                    <th>Montant</th>
                    <th>Devise</th>
                    <th>ReferenceIgor</th>
                    <th>Libelle</th>
                    <th>Operation</th>
                    <th>Expl</th>
                    <th>Etat</th>
                    <th>Date_Regul</th>
                    <th>Action</th>
                  </tr>
              </thead>
              <tbody>
                <?php foreach($vi as $item) { ?>
                  <tr>
                    <td><?php echo $item->DATE_OPER ?></td>
                    <td><?php echo $item->DATE_VAL ?></td>
                    <td><?php echo number_format($item->MONTANT, 0, ',', " ") ?></td>
                    <td><?php echo $item->DEVISE ?></td>
                    <td><?php echo $item->REF_IGOR ?></td>
                    <td><?php echo $item->LIBELLE ?></td>
                    <td ><?php echo $item->OPER ?></td>
                    <td><?php echo $item->EXPL ?></td>
                    <td><?php echo $item->etat ?></td>
                    <td><?php echo $item->date_regul ?></td>
                    <td>
                      <a href="javascript:void(0);" class="text-warning" data-toggle="modal" data-target="#myModal" onClick="modifier('<?php echo $item->id ?>', '<?php echo $item->etat ?>', '<?php echo $item->date_regul ?>')">
                        <box-icon name='printer'  ></box-icon>Modifier
                      </a> 
                    </td>
                  </tr>
                <?php }?>
              </tbody>
          </table>
      </div>
  </div>
</div>
</div>


<form id="modifForm">
  <div class="modal fade" id="myModal">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="card p-3">
          <div class="text-center" style="font-size: 17px;">
            <b>Régulariser</b>
            <button type="button" class="close" data-dismiss="modal">&times;</button>
          </div>
            <div class="row form-group mt-2">
                <div class="col-sm-6" style="display: none;">
                  <label for="ref">ref</label>
                  <input type="text" class="form-control" id="ref_igor" name="ref_igor">
                </div>
                <div class="col-sm-6">
                  <label for="etat">Etat</label>
                  <select name="etat" id="etat" class="form-control">
                    <option value="Non">Non</option>
                    <option value="En cours">En cours</option>
                  </select>
                </div>
                <div class="col-sm-6">
                  <label for="date">Date</label>
                  <input type="date" class="form-control" id="date_regul" name="date_regul" required>
                </div>
                </div>
                <div class="text-center">
                  <button type="button" class="btn btn-danger" data-dismiss="modal">Annuler</button>
                  <input type="submit" class="btn btn-success" value="Valider" />
                </div>
              </div>
            </div>
      </div>
    </div>
  </div>
</form>


<form id="modifFormfr10">
  <div class="modal fade" id="myModalFr10">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="card p-3">
          <div class="text-center" style="font-size: 17px;">
            <b>Fr Dix</b>
            <button type="button" class="close" data-dismiss="modal">&times;</button>
          </div>
            <div class="row form-group mt-2">
                <div class="col-sm-12">
                  <label for="responsable">Résponsable de L'unité</label>
                  <select name="responsable" id="responsable" class="form-control">
                    <option value="hasina">Mme Hasina ANDRIAMAMPIANINA</option>
                    <option value="laza">Mr Lazanirainy RANDRIANAIVOSON</option>
                  </select>
                </div>
                <div class="col-sm-12 mt-1">
                  <label for="nom">Nom initial</label>
                  <input type="text" id="nom" name="nom" class="form-control" required>
                </div>
                <div class="col-sm-12">
                  <label for="compte">Compte client</label>
                  <input type="text" id="compte" name="compte" class="form-control" required>
                </div>
                <div class="col-sm-12">
                  <label for="comm">Compte commission</label>
                  <input type="text" id="comm" name="comm" class="form-control" required>
                </div>
              </div>
                <div class="text-center">
                  <button type="button" class="btn btn-warning" data-dismiss="modal">Annuler</button>
                  <input type="submit" class="btn btn-info" value="Imprimer" />
                </div>
              </div>
            </div>
          </div>
    </div>
  </div>
</form>





<!-- <table id="example" class="display nowrap" style="width:100%">
    <thead>
        <tr>
            <th>Name</th>
            <th>Position</th>
            <th>Office</th>
            <th>Age</th>
            <th>Start date</th>
            <th>Salary</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Tiger Nixon</td>
            <td>System Architect</td>
            <td>Edinburgh</td>
            <td>61</td>
            <td>2011-04-25</td>
            <td>$320,800</td>
        </tr>
        <tr>
            <td>Garrett Winters</td>
            <td>Accountant</td>
            <td>Tokyo</td>
            <td>63</td>
            <td>2011-07-25</td>
            <td>$170,750</td>
        </tr>
        <tr>
            <td>Ashton Cox</td>
            <td>Junior Technical Author</td>
            <td>San Francisco</td>
            <td>66</td>
            <td>2009-01-12</td>
            <td>$86,000</td>
        </tr>
        <tr>
            <td>Cedric Kelly</td>
            <td>Senior Javascript Developer</td>
            <td>Edinburgh</td>
            <td>22</td>
            <td>2012-03-29</td>
            <td>$433,060</td>
        </tr>
        <tr>
            <td>Airi Satou</td>
            <td>Accountant</td>
            <td>Tokyo</td>
            <td>33</td>
            <td>2008-11-28</td>
            <td>$162,700</td>
        </tr>
        <tr>
            <td>Brielle Williamson</td>
            <td>Integration Specialist</td>
            <td>New York</td>
            <td>61</td>
            <td>2012-12-02</td>
            <td>$372,000</td>
        </tr>
        <tr>
            <td>Herrod Chandler</td>
            <td>Sales Assistant</td>
            <td>San Francisco</td>
            <td>59</td>
            <td>2012-08-06</td>
            <td>$137,500</td>
        </tr>
        <tr>
            <td>Rhona Davidson</td>
            <td>Integration Specialist</td>
            <td>Tokyo</td>
            <td>55</td>
            <td>2010-10-14</td>
            <td>$327,900</td>
        </tr>
        <tr>
            <td>Colleen Hurst</td>
            <td>Javascript Developer</td>
            <td>San Francisco</td>
            <td>39</td>
            <td>2009-09-15</td>
            <td>$205,500</td>
        </tr>
        <tr>
            <td>Sonya Frost</td>
            <td>Software Engineer</td>
            <td>Edinburgh</td>
            <td>23</td>
            <td>2008-12-13</td>
            <td>$103,600</td>
        </tr>
        <tr>
            <td>Jena Gaines</td>
            <td>Office Manager</td>
            <td>London</td>
            <td>30</td>
            <td>2008-12-19</td>
            <td>$90,560</td>
        </tr>
        <tr>
            <td>Quinn Flynn</td>
            <td>Support Lead</td>
            <td>Edinburgh</td>
            <td>22</td>
            <td>2013-03-03</td>
            <td>$342,000</td>
        </tr>
        <tr>
            <td>Charde Marshall</td>
            <td>Regional Director</td>
            <td>San Francisco</td>
            <td>36</td>
            <td>2008-10-16</td>
            <td>$470,600</td>
        </tr>
        <tr>
            <td>Haley Kennedy</td>
            <td>Senior Marketing Designer</td>
            <td>London</td>
            <td>43</td>
            <td>2012-12-18</td>
            <td>$313,500</td>
        </tr>
        <tr>
            <td>Tatyana Fitzpatrick</td>
            <td>Regional Director</td>
            <td>London</td>
            <td>19</td>
            <td>2010-03-17</td>
            <td>$385,750</td>
        </tr>
        <tr>
            <td>Michael Silva</td>
            <td>Marketing Designer</td>
            <td>London</td>
            <td>66</td>
            <td>2012-11-27</td>
            <td>$198,500</td>
        </tr>
        <tr>
            <td>Paul Byrd</td>
            <td>Chief Financial Officer (CFO)</td>
            <td>New York</td>
            <td>64</td>
            <td>2010-06-09</td>
            <td>$725,000</td>
        </tr>
        <tr>
            <td>Gloria Little</td>
            <td>Systems Administrator</td>
            <td>New York</td>
            <td>59</td>
            <td>2009-04-10</td>
            <td>$237,500</td>
        </tr>
        <tr>
            <td>Bradley Greer</td>
            <td>Software Engineer</td>
            <td>London</td>
            <td>41</td>
            <td>2012-10-13</td>
            <td>$132,000</td>
        </tr>
        <tr>
            <td>Dai Rios</td>
            <td>Personnel Lead</td>
            <td>Edinburgh</td>
            <td>35</td>
            <td>2012-09-26</td>
            <td>$217,500</td>
        </tr>
        <tr>
            <td>Jenette Caldwell</td>
            <td>Development Lead</td>
            <td>New York</td>
            <td>30</td>
            <td>2011-09-03</td>
            <td>$345,000</td>
        </tr>
        <tr>
            <td>Yuri Berry</td>
            <td>Chief Marketing Officer (CMO)</td>
            <td>New York</td>
            <td>40</td>
            <td>2009-06-25</td>
            <td>$675,000</td>
        </tr>
        <tr>
            <td>Caesar Vance</td>
            <td>Pre-Sales Support</td>
            <td>New York</td>
            <td>21</td>
            <td>2011-12-12</td>
            <td>$106,450</td>
        </tr>
        <tr>
            <td>Doris Wilder</td>
            <td>Sales Assistant</td>
            <td>Sydney</td>
            <td>23</td>
            <td>2010-09-20</td>
            <td>$85,600</td>
        </tr>
        <tr>
            <td>Angelica Ramos</td>
            <td>Chief Executive Officer (CEO)</td>
            <td>London</td>
            <td>47</td>
            <td>2009-10-09</td>
            <td>$1,200,000</td>
        </tr>
        <tr>
            <td>Gavin Joyce</td>
            <td>Developer</td>
            <td>Edinburgh</td>
            <td>42</td>
            <td>2010-12-22</td>
            <td>$92,575</td>
        </tr>
        <tr>
            <td>Jennifer Chang</td>
            <td>Regional Director</td>
            <td>Singapore</td>
            <td>28</td>
            <td>2010-11-14</td>
            <td>$357,650</td>
        </tr>
        <tr>
            <td>Brenden Wagner</td>
            <td>Software Engineer</td>
            <td>San Francisco</td>
            <td>28</td>
            <td>2011-06-07</td>
            <td>$206,850</td>
        </tr>
        <tr>
            <td>Fiona Green</td>
            <td>Chief Operating Officer (COO)</td>
            <td>San Francisco</td>
            <td>48</td>
            <td>2010-03-11</td>
            <td>$850,000</td>
        </tr>
        <tr>
            <td>Shou Itou</td>
            <td>Regional Marketing</td>
            <td>Tokyo</td>
            <td>20</td>
            <td>2011-08-14</td>
            <td>$163,000</td>
        </tr>
        <tr>
            <td>Michelle House</td>
            <td>Integration Specialist</td>
            <td>Sydney</td>
            <td>37</td>
            <td>2011-06-02</td>
            <td>$95,400</td>
        </tr>
        <tr>
            <td>Suki Burks</td>
            <td>Developer</td>
            <td>London</td>
            <td>53</td>
            <td>2009-10-22</td>
            <td>$114,500</td>
        </tr>
        <tr>
            <td>Prescott Bartlett</td>
            <td>Technical Author</td>
            <td>London</td>
            <td>27</td>
            <td>2011-05-07</td>
            <td>$145,000</td>
        </tr>
        <tr>
            <td>Gavin Cortez</td>
            <td>Team Leader</td>
            <td>San Francisco</td>
            <td>22</td>
            <td>2008-10-26</td>
            <td>$235,500</td>
        </tr>
        <tr>
            <td>Martena Mccray</td>
            <td>Post-Sales support</td>
            <td>Edinburgh</td>
            <td>46</td>
            <td>2011-03-09</td>
            <td>$324,050</td>
        </tr>
        <tr>
            <td>Unity Butler</td>
            <td>Marketing Designer</td>
            <td>San Francisco</td>
            <td>47</td>
            <td>2009-12-09</td>
            <td>$85,675</td>
        </tr>
    </tbody>
    <tfoot>
        <tr>
            <th>Name</th>
            <th>Position</th>
            <th>Office</th>
            <th>Age</th>
            <th>Start date</th>
            <th>Salary</th>
        </tr>
    </tfoot>
</table> -->



<script>

function modifier(ref_igor, etat, date) {
    $("#ref_igor").val(ref_igor);
    $("#etat").val(etat);
    $("#date_regul").val(date);
  }


$(document).ready(function(){

  $("#tableNormaleCashOut").DataTable({
    responsive: true,
    "scrollX": true, // Active la barre de défilement horizontal
    "autoWidth": false,
    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
  });

    // $('#tableNormaleCashIn').DataTable( {
    //     dom: 'Bfrtip',
    //     responsive: true,
    //     scrollX: true,
    //     buttons: [
    //         'copy', 'csv', 'excel', 'pdf', 'print'
    //         ],
    //     language: {
    //       url: '<?php echo base_url(); ?>assets/fr-FR.json',
    // }
    // });

  $("#tableDat").DataTable({
    responsive: true,
    "scrollX": true,
    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
  });

  $("#tableCat").DataTable({
    responsive: true,
    "scrollX": true,

    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }

  });

  $("#tableDeallo").DataTable({
    responsive: true,
    "scrollX": true,

    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }

  });

  $("#tableAdmin").DataTable({
    responsive: true,
    "scrollX": true,

    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }

  });

  $("#tableVI").DataTable({
    responsive: true,
    "scrollX": true,

    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }

  });
  


    $("#tableAnormaleCashOut").DataTable();
  $(".nav-tabs a").click(function(){
    $(this).tab('show');
    // Ajoutez le code pour masquer la tab "Table Normale Cash In" ici
    if($(this).attr('href') === '#anormale') {
      $("#tabNormaleCashIn").removeClass("show active");
      $("#tabNormaleCashOut").removeClass("show active");
    }
    if($(this).attr('href') === '#normale') {
      $("#tabDat").removeClass("show active");
      $("#tabCat").removeClass("show active");
    }
  });
});
</script>
<script>
  let minDate, maxDate;
 DataTable.ext.search.push(function (settings, data, dataIndex) {
     let min = minDate.val();
     let max = maxDate.val();
     let date = new Date(data[0]);
  
     if (
         (min === null && max === null) ||
         (min === null && date <= max) ||
         (min <= date && max === null) ||
         (min <= date && date <= max)
     ) {
         return true;
     }
     return false;
 });
  
 // Create date inputs
 minDate = new DateTime('#min', {
    format: 'LL', 
    locale: 'fr', 
});

maxDate = new DateTime('#max', {
    format: 'LL', 
    locale: 'fr', 
});
  
 // DataTables initialisation
 let table = new DataTable('#tableNormaleCashIn', {
  scrollX: true,
  language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
 }
 );
  

 document.querySelectorAll('#min, #max').forEach((el) => {
     el.addEventListener('change', () => table.draw());
 });
 

</script>

<script>
   function ouvrirEtImprimer(date_oper, ref_igor, montant, oper, compte) {

var url = '<?php echo site_url('Imprimer_Telma/imprimerDocument'); ?>' +
  '?date=' + encodeURIComponent(date_oper) +
  '&ref=' + encodeURIComponent(ref_igor) +
  '&oper=' + encodeURIComponent(oper)+
  '&montant=' + encodeURIComponent(montant)+
  '&compte=' + encodeURIComponent(compte);

var nouvelleFenetre = window.open(url, '_blank');

}

var compte, dateOper, montant, libelle, oper, refIgor;
  $("#myModalFr10").on('show.bs.modal', function(event) {
    link = $(event.relatedTarget);
    compte = link.data('compte');
    dateOper = link.data('date-oper');
    montant = link.data('montant').toLocaleString('fr-FR');
    libelle = link.data('libelle');
    oper = link.data('oper');
    refIgor = link.data('ref-igor');
});

$("#modifFormfr10").submit(function(e) {
    e.preventDefault();
    var responsable = $("#responsable").val();
    const nom= $("#nom").val();
    const compte_client= $("#compte").val();
    const comm= $("#comm").val();
    let place;

    if(responsable === "hasina") {
      responsable = document.getElementById("responsable").options[ document.getElementById("responsable").selectedIndex].text;
      place= "Directeur Déléguée à la Monétique";
    } else {
      responsable = document.getElementById("responsable").options[ document.getElementById("responsable").selectedIndex].text;
      place= " Directeur Déléguée à la Monétique (p.i)";
    }

    // console.log(compte, dateOper, montant, libelle, oper, refIgor);

    var url = '<?php echo site_url('Imprimer/imprimerDocument'); ?>' +
      '?date=' + encodeURIComponent(dateOper) +
      '&ref=' + encodeURIComponent(refIgor) +
      '&oper=' + encodeURIComponent(oper)+
      '&montant=' + encodeURIComponent(montant)+
      '&libelle=' + encodeURIComponent(libelle)+
      '&nom=' + encodeURIComponent(nom) +
      '&responsable=' + encodeURIComponent(responsable) + 
      '&compte_client=' + encodeURIComponent(compte_client) + 
      '&compte_comm=' + encodeURIComponent(comm)+
      '&compte=' + encodeURIComponent(compte)+
      "&operateur="  + encodeURIComponent("MVL") +
      "&mobile_money=" + encodeURIComponent("M VOLA") +
      "&tax="  + encodeURIComponent("333,33") +
      "&tva=" + encodeURIComponent("66,67") +
      "&montant_comm=" + encodeURIComponent("400,00") +
      "&place=" + encodeURIComponent(place)
    var nouvelleFenetre = window.open(url, '_blank'); 

});

</script>
</script>
