#include "TestBladeFlurry.h"

#include "Backstab.h"
#include "BladeFlurry.h"
#include "Combat.h"
#include "Equipment.h"
#include "MainhandAttack.h"
#include "RogueSpells.h"
#include "Talent.h"
#include "WarriorSpells.h"

TestBladeFlurry::TestBladeFlurry(EquipmentDb *equipment_db) :
    TestSpellRogue(equipment_db, "Blade Flurry")
{}

void TestBladeFlurry::test_all() {
    run_mandatory_tests(false);

    set_up(false);
    test_combo_points();
    tear_down();

    set_up(false);
    test_stealth();
    tear_down();

    set_up(false);
    test_attack_speed();
    tear_down();
}

BladeFlurry *TestBladeFlurry::blade_flurry() {
    return dynamic_cast<RogueSpells*>(rogue->get_spells())->get_blade_flurry();
}

void TestBladeFlurry::test_name_correct() {
    assert(blade_flurry()->get_name() == "Blade Flurry");
}

void TestBladeFlurry::test_spell_cooldown() {
    assert(QString::number(blade_flurry()->get_base_cooldown(), 'f', 3) == "120.000");
}

void TestBladeFlurry::test_incurs_global_cooldown() {
    given_1_of_1_blade_flurry();
    when_blade_flurry_is_performed();

    then_next_event_is("MainhandMeleeHit", "0.000");
    then_next_event_is("PlayerAction", QString::number(rogue->global_cooldown(), 'f', 3));
}

void TestBladeFlurry::test_obeys_global_cooldown() {
    given_1_of_1_blade_flurry();
    assert(blade_flurry()->is_available());

    given_rogue_is_on_gcd();

    assert(!blade_flurry()->is_available());
}

void TestBladeFlurry::test_is_ready_conditions() {

}

void TestBladeFlurry::test_resource_cost() {
    given_1_of_1_blade_flurry();
    given_rogue_has_energy(0);
    assert(blade_flurry()->is_available());

    when_blade_flurry_is_performed();

    then_rogue_has_energy(0);
}

void TestBladeFlurry::test_combo_points() {
    given_1_of_1_blade_flurry();

    given_rogue_has_combo_points(0);
    assert(blade_flurry()->is_available());

    given_rogue_has_combo_points(1);
    assert(blade_flurry()->is_available());

    given_rogue_has_combo_points(2);
    assert(blade_flurry()->is_available());

    given_rogue_has_combo_points(3);
    assert(blade_flurry()->is_available());

    given_rogue_has_combo_points(4);
    assert(blade_flurry()->is_available());

    given_rogue_has_combo_points(5);
    assert(blade_flurry()->is_available());

    when_blade_flurry_is_performed();
    then_rogue_has_combo_points(5);
}

void TestBladeFlurry::test_stealth() {
    given_1_of_1_blade_flurry();
    given_rogue_not_in_stealth();
    assert(blade_flurry()->is_available());
    given_rogue_in_stealth();
    assert(blade_flurry()->is_available());

    when_blade_flurry_is_performed();

    assert(!rogue->is_stealthed());
}

void TestBladeFlurry::test_attack_speed() {
    given_1_of_1_blade_flurry();
    given_a_mainhand_weapon_with_2_speed();
    given_no_offhand();
    ignored_events = {"PlayerAction"};

    when_blade_flurry_is_performed();

    rogue->get_spells()->start_attack();
    then_next_event_is("MainhandMeleeHit", "0.000", RUN_EVENT);
    then_next_event_is("MainhandMeleeHit", "0.000", RUN_EVENT);
    then_next_event_is("MainhandMeleeHit", "1.667", RUN_EVENT);
    then_next_event_is("MainhandMeleeHit", "3.333", RUN_EVENT);
    then_next_event_is("MainhandMeleeHit", "5.000", RUN_EVENT);
    then_next_event_is("MainhandMeleeHit", "6.667", RUN_EVENT);
    then_next_event_is("MainhandMeleeHit", "8.333", RUN_EVENT);
    then_next_event_is("MainhandMeleeHit", "10.000", RUN_EVENT);
    then_next_event_is("MainhandMeleeHit", "11.667", RUN_EVENT);
    then_next_event_is("MainhandMeleeHit", "13.333", RUN_EVENT);
    then_next_event_is("MainhandMeleeHit", "15.000", RUN_EVENT);
    then_next_event_is("BuffRemoval", "15.000", RUN_EVENT);
    then_next_event_is("MainhandMeleeHit", "16.667", RUN_EVENT);
    then_next_event_is("MainhandMeleeHit", "17.000", RUN_EVENT);
    then_next_event_is("MainhandMeleeHit", "19.000");
}

void TestBladeFlurry::when_blade_flurry_is_performed() {
    if (pchar->get_equipment()->get_mainhand() == nullptr)
        given_a_mainhand_dagger_with_100_min_max_dmg();

    blade_flurry()->perform();
}

void TestBladeFlurry::given_1_of_1_blade_flurry() {
    Talent* talent = Combat(rogue).get_blade_flurry();

    assert(talent->increment_rank());

    rogue->prepare_set_of_combat_iterations();

    delete talent;
}
